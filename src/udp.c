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
	char    msg[(sizeof(SbkHighFb)<<1) + 1]; // Using SbkHighFb as it has! the biggest size
	size_t  strSize;

	strSize = size<<1;
	
	for (int i = 0; i < strSize; i += 2)
		sprintf(&(msg[i]), "%02X", bytes[i>>1]);

	msg[strSize] = '\0';
	
	sbk_sync_printf("%s\n", msg);
}


ssize_t
sbk_udp_open(const uint8_t  level,
			 const bool     encode,
			 const bool     useWifi,
			 const int      socketType,
			 SbkConnection  *conn)
{
	SbkLowCtrl   emptyMsg = {0};
	u_short      port;
	int          sendSize, recvSize, fd;
	in_addr_t    addr;
	uint32_t     (*gen_code)(const uint32_t *, const size_t);

	if (level == SBK_UDP_LOW_LEVEL_CONN) {
		sendSize = sizeof(SbkLowCtrl);
		recvSize = sizeof(SbkLowFb);
		port     = SBK_UDP_LOW_LEVEL_PORT;
		addr     = SBK_MCU_ADDR;
		gen_code = __SBK_gen_crc_encode;
		
	} else {
		sendSize = sizeof(SbkHighCtrl);
		recvSize = sizeof(SbkHighFb);
		port     = SBK_UDP_HIGH_LEVEL_PORT;
		addr     = useWifi ? SBK_PI_WIFI_ADDR : SBK_PI_ADDR;
		gen_code = encode ? __SBK_gen_crc_encode
		                  : __SBK_gen_crc;
	}

	// Set up SbkConnection instance	
	fd = socket(AF_INET, SOCK_DGRAM | socketType, 0);

	conn->fd       = fd;
	conn->gen_code = gen_code;
	conn->sendSize = sendSize;
	conn->recvSize = recvSize;
	
	conn->addr.sin_addr.s_addr = addr;
	conn->addr.sin_port        = htons(port);
	conn->addr.sin_family      = AF_INET;

	atomic_flag_clear(&(conn->lock));

	// Initialize the connecion by sending an empty command
	emptyMsg.levelFlag = level;
	emptyMsg.head      = SBK_UDP_MSG_HEADER;
	if (level == SBK_UDP_LOW_LEVEL_CONN)
		emptyMsg.bandwidth = SBK_LOW_LEVEL_BANDWIDTH;

	((uint8_t *) &emptyMsg)[sendSize-4] = gen_code((uint32_t *) &emptyMsg, sendSize);
		
	return sbk_udp_send(conn, (uint8_t *) &emptyMsg);
}


ssize_t
__SBK_udp_send(SbkConnection  *conn,
	  		   uint8_t        *data)
{
	ssize_t bytesSent, sendSize;
	atomic_flag *lock;

	sendSize = conn->sendSize;
	*((uint32_t *) &(data[sendSize-4])) = conn->gen_code((uint32_t *)data, sendSize);

	lock = &(conn->lock);
	if (atomic_flag_test_and_set_explicit(lock, memory_order_acquire)) {
#ifdef DEBUG
		sbk_sync_puts("!! Failed to acquire socket lock\n");
#endif
		return -1;
	}
	
	bytesSent = sendto(conn->fd, data, sendSize, 0,
					   (struct sockaddr *) &(conn->addr),
					   sizeof(struct sockaddr_in));

	atomic_flag_clear_explicit(lock, memory_order_release);
	
#ifdef DEBUG
	sbk_print_raw_msg(data, sendSize);
	sbk_sync_printf("-- Bytes sent: %ld (intended: %ld)\n",
					bytesSent, sendSize);
#endif
	
	return bytesSent;
}


ssize_t
__SBK_udp_recv(SbkConnection  *conn,
			   void           *buf)
{
	socklen_t addrlen;
	ssize_t recieved;
	atomic_flag *lock;

	addrlen = sizeof(struct sockaddr_in);

	lock = &(conn->lock);
	if (atomic_flag_test_and_set_explicit(lock, memory_order_acquire)) {
#ifdef DEBUG
		sbk_sync_puts("!! Failed to acquire socket lock\n");
#endif
		return -1;
	}
	
	recieved = recvfrom(conn->fd, buf, conn->recvSize, 0,
						(struct sockaddr *) &(conn->addr), &addrlen);

	atomic_flag_clear_explicit(lock, memory_order_release);

#ifdef DEBUG
	sbk_sync_printf("-- Received bytes: %ld (intended: %ld)\n",
					recieved, conn->recvSize);
#endif
	
	return recieved;
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
		while (atomic_flag_test_and_set_explicit(
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
