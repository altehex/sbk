#ifndef __SBK_UDP_H
#define __SBK_UDP_H


#include <stdatomic.h>
#include <netinet/in.h>
#include <sys/socket.h>


#define SBK_UDP_LISTEN_PORT     8090
#define SBK_UDP_HIGH_LEVEL_PORT 8082
#define SBK_UDP_LOW_LEVEL_PORT  8007

typedef enum {
	SBK_UDP_HIGH_LEVEL_CONN,
	SBK_UDP_LOW_LEVEL_CONN
} SbkConnType;

#define SBK_OCTETS_TO_ADDR(a, b, c, d) \
	(   ( (d&0xFF) << 24 ) \
      | ( (c&0xFF) << 16 ) \
      | ( (b&0xFF) << 8 )  \
      |   (a&0xFF)         )

#define SBK_MCU_ADDR         ((in_addr_t) SBK_OCTETS_TO_ADDR(192,168,123,10))
#define SBK_PI_ADDR          ((in_addr_t) SBK_OCTETS_TO_ADDR(192,168,123,161))
#define SBK_WIFI_LISTEN_IP   ((in_addr_t) SBK_OCTETS_TO_ADDR(192,168,12,14))
#define SBK_WIRED_LISTEN_IP  ((in_addr_t) SBK_OCTETS_TO_ADDR(192,168,123,16))


typedef struct {
	int                 fd;
	struct sockaddr_in  addr;
	pthread_mutex_t     mtx;
	size_t              msgSize;
} SbkConnection;


#define SBK_UDP_MSG_QUEUE_LENGTH 50

// Circular buffer (deprecated)
typedef struct {
	void         *offset, *head;
	size_t       msgSize, length, free;
	atomic_flag  lock;
	int          available;
} SbkMsgQueue;


ssize_t
sbk_udp_open(SbkConnType type, const size_t queueLength, SbkConnection *conn, SbkMsgQueue **queue);
void sbk_udp_close(SbkConnection *conn, SbkMsgQueue *queue);
ssize_t sbk_udp_send(SbkConnection * sock, uint8_t *data, const size_t size);
ssize_t sbk_udp_recv(SbkConnection *conn, void *buf, const size_t recvSize);

#if 0
int sbk_udp_get_msgs(SbkMsgQueue *queue, void *, const size_t);
int sbk_udp_try_get_msgs(SbkMsgQueue *queue, void *, const size_t);
void *sbk_udp_recv_loop(void *);
#endif


typedef struct {
	SbkConnection *conn;
	SbkMsgQueue   *queue;
	ssize_t       recvLength;
} SbkRecvLoopArgs;


#endif // !__SBK_UDP_H
