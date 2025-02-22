#include <sbk/udp.h>

#include <sbk/high_level.h>
#include <sbk/low_level.h>
#include <sbk/macro.h>
#include <sbk/syncio.h>

#include <errno.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


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


int
__SBK_setup_udp_socket(const in_addr_t     addr,
					   struct sockaddr_in  *addrOut,
					   const u_short       port)
{
	int   fd;
	char  addrStr[INET_ADDRSTRLEN];
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (fd == -1) {
		sbk_sync_printf("!! Bad socket\n");
		return -1;
	}

	addrOut->sin_addr.s_addr = addr;
	addrOut->sin_port        = htons(port);
	addrOut->sin_family      = AF_INET;
	
	inet_ntop(AF_INET, &(addrOut->sin_addr),
			  addrStr, INET_ADDRSTRLEN); 

	if (connect(fd, (struct sockaddr *) addrOut,
				sizeof(struct sockaddr_in)) == -1) {
		sbk_sync_printf("!! Failed to connect to %s:%d\n"
				   "!! errno: %d\n",
				   addrStr, port,
				   errno);
		return -1;
	}
	
#ifdef DEBUG
	__SBK_debug_udp_print_conn(fd, addrOut);
#endif

	return fd;
}

ssize_t
sbk_udp_open(const u_short    recvPort,
			 const in_addr_t  recvAddr,
			 SbkConnType      type,
			 const ssize_t    queueLength,
			 SbkConnection    *conn,
			 SbkMsgQueue      **queue)
{
	SbkMsgQueue  *qp;
	uint8_t      emptyMsg[sizeof(SbkLowCtrl)];
	u_short      sendPort;
	int          msgSize, qLen, sendfd, recvfd;
	in_addr_t    sendAddr;

	if (type == SBK_UDP_LOW_LEVEL_CONN) {
		msgSize  = sizeof(SbkLowCtrl);
		sendPort = SBK_UDP_LOW_LEVEL_PORT;
		sendAddr = SBK_MCU_ADDR;
	} else {
		msgSize  = sizeof(SbkHighCtrl);
		sendPort = SBK_UDP_HIGH_LEVEL_PORT;
		sendAddr = SBK_PI_ADDR;
	}

	// Set up SbkConnection instance
	sendfd = __SBK_setup_udp_socket(sendAddr, &(conn->send), sendPort);
	recvfd = __SBK_setup_udp_socket(recvAddr, &(conn->recv), recvPort);

	if (sendfd == -1 || recvfd == -1)
		return -1;

	conn->sendfd = sendfd;
	conn->recvfd = recvfd;
	
	// Set up message queue if needed
	if (queue) {
		qLen = queueLength ? queueLength : SBK_UDP_MSG_QUEUE_LENGTH;
		qp = (SbkMsgQueue *) malloc(sizeof(SbkMsgQueue));

		qp->length    = qLen;
		qp->msgSize   = msgSize;
		qp->head      = (uint8_t *) malloc(msgSize*qLen);
		qp->offset    = 0;
		qp->free      = 0;
		qp->lock      = (atomic_flag) {false};
		qp->available = false;

		(*queue) = qp;
	}
	
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
sbk_udp_close(SbkConnection  *conn,
			  SbkMsgQueue    *queue)
{	
	close(conn->recvfd);
	close(conn->sendfd);
	
	if (queue) {
		free(queue->head);
		free(queue);
	}
}


ssize_t
sbk_udp_send(const SbkConnection  *conn,
			 const uint8_t        *data,
			 const unsigned int   size)
{
#ifdef DEBUG
	ssize_t bytesSent = sendto(conn->sendfd, data, size-1, 0,
							   (struct sockaddr *) &(conn->send),
							   sizeof(struct sockaddr_in));
	__SBK_debug_udp_print_conn(conn->sendfd, &(conn->send));
	sbk_sync_printf("-- Bytes sent: %ld (intended: %ld)\n",
					bytesSent, size);
	return bytesSent;
#endif
	
	return sendto(conn->sendfd, data, size, 0,
				  (struct sockaddr *) &(conn->send),
		          sizeof(struct sockaddr_in));
}


// !!! intrinsic, not thread safe
int
__SBK_retrieve_from_queue(SbkMsgQueue   *queue,
				          void          *buf,
						  const ssize_t length)
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
				 const ssize_t length)
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
					 const ssize_t length)
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


ssize_t
sbk_udp_recv(const SbkConnection  *conn,
			 void                 *buf,
			 const ssize_t        recvSize)
{
	socklen_t addrlen = sizeof(struct sockaddr_in);
	
#ifdef DEBUG
	recvfrom(conn->recvfd, buf, recvSize,
			 0, (struct sockaddr *) &(conn->recv),
			 &addrlen);

	sbk_sync_printf("-- Received bytes: %ld\n", addrlen);
	return addrlen;
#endif
	
	return recvfrom(conn->recvfd, buf, recvSize,
					0, (struct sockaddr *) &(conn->recv),
					&addrlen);
}


/*************************
  
typedef struct {
	SbkConnection *conn;
	SbkMsgQueue   *queue;
	ssize_t       recvLength;
} SbkRecvLoopArgs;

*************************/

void *
sbk_udp_recv_loop(void *args)
{
	SbkRecvLoopArgs *a;
	SbkMsgQueue *queue;
	unsigned long head, offset;
	ssize_t msgSize, length, *free, recvLength,
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

