#include <sbk/udp.h>

#include <sbk/crc.h>
#include <sbk/high_level.h>
#include <sbk/low_level.h>
#include <sbk/macro.h>
#include <sbk/syncio.h>

#include <errno.h> 
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


void
__SBK_print_raw_msg(const uint8_t  *bytes,
					const size_t   size)
{
	char *msg;

	msg = (char *) malloc(size<<1);
	
	for (int i = 0; i < size<<1; i += 2)
		sprintf(&(msg[i]), "%02X", ((uint8_t *)bytes)[i>>1]);

	sbk_sync_printf("%s\n", msg);

	free(msg);
}


void
__SBK_debug_udp_print_conn(const int fd,
						   struct sockaddr_in  *addrOut)
{
	char addrStr[INET_ADDRSTRLEN];

	inet_ntop(AF_INET, &(addrOut->sin_addr),
			  addrStr, INET_ADDRSTRLEN); 

	sbk_sync_printf("-- sockfd: %ld\n"
					"-- address: %s:%d\n",
					fd, addrStr, ntohs(addrOut->sin_port));
}

ssize_t
sbk_udp_open(SbkConnType    type,
			 SbkConnection  *conn,
			 const bool     encode,
			 const int      socketType)
{
	uint8_t      emptyMsg[sizeof(SbkLowCtrl)];
	u_short      port;
	int          msgSize, qLen, fd;
	in_addr_t    addr;
	char         addrStr[INET_ADDRSTRLEN];

	if (type == SBK_UDP_LOW_LEVEL_CONN) {
		msgSize = sizeof(SbkLowCtrl);
		port    = SBK_UDP_LOW_LEVEL_PORT;
		addr    = SBK_MCU_ADDR;
	} else {
		msgSize  = sizeof(SbkHighCtrl);
		port     = SBK_UDP_HIGH_LEVEL_PORT;
		addr     = SBK_PI_ADDR;
	}

	// Set up SbkConnection instance	
	fd = socket(AF_INET, SOCK_DGRAM | socketType, 0);

	if (fd == -1) {
		sbk_sync_printf("!! Bad socket\n");
		return -1;
	}

	conn->addr.sin_addr.s_addr = addr;
	conn->addr.sin_port        = htons(port);
	conn->addr.sin_family      = AF_INET;

	conn->fd      = fd;
	conn->msgSize = msgSize;
	pthread_mutex_init(&(conn->mtx), NULL);

	// TO-DO: test if low level can be accessed with
	//        decoded crc
	if (encode)
		conn->gen_code = __SBK_gen_crc_encode;
	else
		conn->gen_code = __SBK_gen_crc;

	if (connect(fd, (struct sockaddr *) &(conn->addr),
				sizeof(struct sockaddr_in)) == -1) {
		inet_ntop(AF_INET, &(conn->addr.sin_addr),
				  addrStr, INET_ADDRSTRLEN);
		
		sbk_sync_printf("!! Failed to connect to %s:%d\n"
				   "!! errno: %d\n",
				   addrStr, port,
				   errno);
		return -1;
	}
	
#ifdef DEBUG
	__SBK_debug_udp_print_conn(fd, &(conn->addr));
#endif
	
	// Initialize the connecion by sending an empty command
	memset(emptyMsg, 0, msgSize);

#ifdef DEBUG
	ssize_t bytesSent = sbk_udp_send(conn, emptyMsg, msgSize);
	sbk_sync_puts("-- Iniitalized connection");
	return bytesSent;
#endif
	
	return sbk_udp_send(conn, emptyMsg, msgSize);
}


void
sbk_udp_close(SbkConnection  *conn)
{	
	close(conn->fd);
}


ssize_t
sbk_udp_send(SbkConnection  *conn,
			 uint8_t        *data,
			 const size_t   size)
{
	ssize_t bytesSent;
	int lockStatus;

	data[size-4] = conn->gen_code((uint32_t *)data, size);
	
	lockStatus = pthread_mutex_trylock(&(conn->mtx));
	
	if (lockStatus != 0) {
#ifdef DEBUG
		printf("!! Failed to acquire socket lock\n"
						"!! errno: %d\n",
						lockStatus);
#endif
		return -1;
	}
	
	bytesSent = sendto(conn->fd, data, size, 0,
					   (struct sockaddr *) &(conn->addr),
					   sizeof(struct sockaddr_in));

	lockStatus = pthread_mutex_unlock(&(conn->mtx));
	
#ifdef DEBUG
	__SBK_debug_udp_print_conn(conn->fd, &(conn->addr));
	sbk_sync_printf("-- Bytes sent: %ld (intended: %ld)\n",
					bytesSent, size);
#endif
	
	return bytesSent;
}


ssize_t
sbk_udp_recv(SbkConnection  *conn,
			 void           *buf,
			 const size_t   recvSize)
{
	socklen_t addrlen = sizeof(struct sockaddr_in);
	int lockStatus;

	lockStatus = pthread_mutex_trylock(&(conn->mtx));
	
	if (lockStatus != 0) {
#ifdef DEBUG
		printf("!! Failed to acquire socket lock\n"
						"!! errno: %d\n",
						lockStatus);
#endif
		return -1;
	}
	
	recvfrom(conn->fd, buf, recvSize, 0,
			 (struct sockaddr *) &(conn->addr), &addrlen);

	pthread_mutex_unlock(&(conn->mtx));

#ifdef DEBUG
	sbk_sync_printf("-- Received bytes: %ld\n", addrlen);
#endif
	
	return addrlen;
}


#if 0 // Have to reimplement message queue once it'll start to work properly

/*************************
  
typedef struct {
	SbkConnection *conn;
	SbkMsgQueue   *queue;
	size_t        recvLength;
} SbkRecvLoopArgs;

*************************/

void *
sbk_udp_recv_loop(void *args)
{
	SbkRecvLoopArgs *a;
	SbkMsgQueue *queue;
	unsigned long head, offset;
	size_t msgSize, length, *free, recvLength,
		   bytesRead;
	atomic_flag *lock;

	a = (SbkRecvLoopArgs *) args;
	recvLength = a->recvLength;
	
	queue = a->queue;
	lock    = &(queue->lock);
	free    = &(queue->free);
	head    = (unsigned long) queue->head;
	msgSize = (unsigned long) queue->msgSize;
	length  = queue->length;
	offset  = (unsigned long) queue->offset;

	while (1) {
		while ( ! atomic_flag_test_and_set_explicit(
			       lock, memory_order_acquire)) { continue; }

		if (*free) {
			recvLength = __MIN(*free, recvLength);

			if (recvLength >= length-offset) {
				if (length != offset) {
					bytesRead = sbk_udp_recv(a->conn, (void *) (head+offset),
											 (length-offset)*msgSize);
					if (bytesRead < length-offset) {
						offset += bytesRead;
						queue->available = 1;
						atomic_flag_clear_explicit(lock, memory_order_release);
						goto release;
					}
					
					recvLength -= bytesRead;
					*free -= bytesRead;
				}
				
				bytesRead = sbk_udp_recv(a->conn, (void *) (head),
										 recvLength*msgSize);
				if (bytesRead < recvLength) {
					offset = bytesRead;
					queue->available = 1;
					goto release;
				}
				
				offset = recvLength-bytesRead;
			} else {
				bytesRead = sbk_udp_recv(a->conn, (void *) (head+offset),
										 recvLength*msgSize);
				offset += bytesRead;
			}
			
			queue->available = 1;
			*free -= bytesRead;
		}

release:
		atomic_flag_clear_explicit(lock, memory_order_release);
	}
	
	queue->offset = (void *) offset;

	return NULL;
}


// !!! intrinsic, not thread safe
int
__SBK_retrieve_from_queue(SbkMsgQueue   *queue,
				          void          *buf,
						  const size_t  length)
{
	unsigned long head, offset;
	ssize_t retrieved;

	if ( ! queue->available)
		return 0;
	
	retrieved = __MIN(length, (queue->length)-(queue->free));
	
	head   = (unsigned long) queue->head;
	offset = (unsigned long) queue->offset;
	
	memmove(buf, (void *) (head+offset),
			retrieved*(queue->msgSize));

	queue->free += retrieved;
	
	return retrieved;
}

int
sbk_udp_get_msgs(SbkMsgQueue   *queue,
				 void          *buf,
				 const size_t  length)
{
	atomic_flag *lock;
	ssize_t retrieved;

	lock = &(queue->lock);
	
	while ( ! atomic_flag_test_and_set_explicit(
			  lock, memory_order_acquire)) { continue; };

	retrieved = __SBK_retrieve_from_queue(queue, buf, length);
		
	atomic_flag_clear_explicit(lock, memory_order_release);
	
	return retrieved;
}

int
sbk_udp_try_get_msgs(SbkMsgQueue   *queue,
				     void          *buf,
					 const size_t  length)
{
	atomic_flag *lock;
	ssize_t retrieved;

	lock = &(queue->lock);
	
	if ( ! atomic_flag_test_and_set_explicit(
			  lock, memory_order_acquire)) return 1;
	
	retrieved = __SBK_retrieve_from_queue(queue, buf, length);
	
	atomic_flag_clear_explicit(lock, memory_order_release);
	
	return retrieved;
}

#endif
