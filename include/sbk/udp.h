#ifndef __SBK_UDP_H
#define __SBK_UDP_H


#include <stdatomic.h>
#include <stdbool.h>
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

#define SBK_MCU_ADDR  ((in_addr_t) SBK_OCTETS_TO_ADDR(192,168,123,10))
#define SBK_PI_ADDR   ((in_addr_t) SBK_OCTETS_TO_ADDR(192,168,123,161))


typedef struct {
	int                 fd;
	struct sockaddr_in  addr;
	pthread_mutex_t     mtx;
	size_t              msgSize;
	uint32_t (*gen_code)(const uint32_t *, size_t);
} SbkConnection;


ssize_t
sbk_udp_open(SbkConnType type, SbkConnection *conn, const bool encode, const int socketType);
void sbk_udp_close(SbkConnection *conn);
ssize_t sbk_udp_send(SbkConnection * sock, uint8_t *data, const size_t size);
ssize_t sbk_udp_recv(SbkConnection *conn, void *buf, const size_t recvSize);

void __SBK_print_raw_msg(const uint8_t *, const size_t);

#define sbk_print_raw_msg(BYTES, SIZE) \
	__SBK_print_raw_msg((uint8_t *) BYTES, SIZE)


#if 0

#define SBK_UDP_MSG_QUEUE_LENGTH 50

// Circular buffer (deprecated)
typedef struct {
	void         *offset, *head;
	size_t       msgSize, length, free;
	atomic_flag  lock;
	int          available;
} SbkMsgQueue;


typedef struct {
	SbkConnection *conn;
	SbkMsgQueue   *queue;
	ssize_t       recvLength;
} SbkRecvLoopArgs;


int sbk_udp_get_msgs(SbkMsgQueue *queue, void *, const size_t);
int sbk_udp_try_get_msgs(SbkMsgQueue *queue, void *, const size_t);
void *sbk_udp_recv_loop(void *);

#endif // !0


#endif // !__SBK_UDP_H
