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


extern struct sockaddr_in sbkMcuAddr, sbkPiAddr;


typedef struct {
	int                 sendfd, recvfd;
	struct sockaddr_in  send, recv;
} SbkConnection;


#define SBK_UDP_MSG_QUEUE_LENGTH 50

// Circular buffer
typedef struct {
	void         *offset, *head;
	ssize_t      msgSize, length, free;
	atomic_flag  lock;
	int          available;
} SbkMsgQueue;


ssize_t
sbk_udp_open(const u_short recvPort, const in_addr_t recvAddr, SbkConnType type, const ssize_t queueLength, SbkConnection *conn, SbkMsgQueue **queue);
void sbk_udp_close(SbkConnection *conn, SbkMsgQueue *queue);
ssize_t sbk_udp_send(const SbkConnection * sock, const uint8_t *data, const unsigned int size);
int sbk_udp_get_msgs(SbkMsgQueue *queue, void *, const ssize_t);
int sbk_udp_try_get_msgs(SbkMsgQueue *queue, void *, const ssize_t);
ssize_t sbk_udp_recv(const SbkConnection *conn, void *buf, const ssize_t recvSize);
void *sbk_udp_recv_loop(void *);


typedef struct {
	SbkConnection *conn;
	SbkMsgQueue   *queue;
	ssize_t       recvLength;
} SbkRecvLoopArgs;


#endif // !__SBK_UDP_H
