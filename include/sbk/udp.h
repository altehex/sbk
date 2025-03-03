/**
   @file
   @brief UDP communication function definitions
  */

#ifndef __SBK_UDP_H
#define __SBK_UDP_H


#include <sbk/macro.h>

#include <stdatomic.h>
#include <stdbool.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>


#define SBK_UDP_MSG_HEADER 0xEFFE

#define SBK_UDP_LISTEN_PORT      8090
#define SBK_UDP_HIGH_LEVEL_PORT  8082
#define SBK_UDP_LOW_LEVEL_PORT   8007

#define SBK_UDP_HIGH_LEVEL_CONN  0x00
#define SBK_UDP_LOW_LEVEL_CONN   0xFF

#define SBK_OCTETS_TO_ADDR(a, b, c, d) \
	( (in_addr_t) (   ( (d&0xFF) << 24 ) \
                    | ( (c&0xFF) << 16 ) \
                    | ( (b&0xFF) << 8 )  \
                    |   (a&0xFF)         ) )

#define SBK_PI_WIFI_ADDR  SBK_OCTETS_TO_ADDR(192,168,12,1)
#define SBK_MCU_ADDR      SBK_OCTETS_TO_ADDR(192,168,123,10)
#define SBK_PI_ADDR       SBK_OCTETS_TO_ADDR(192,168,123,161)


typedef struct {
	int                 fd;
	size_t              sendSize;
	size_t              recvSize;
	struct sockaddr_in  addr;
	atomic_flag         lock;
	uint32_t            (*gen_code)(const uint32_t *, const size_t);
} SbkConnection;


/**
  @brief Establishes UDP connection corresponding to either low or high level communication.
  
  @param[in]  level       Communication level (SBK_UDP_HIGH_LEVEL_CONN or SBK_UDP_LOW_LEVEL_CONN)
  @param[in]  encode      Whether the messages are encoded when sent (encoding refers to applying the so-called encryption algorithm by the Unitree devs, see sbk_gen_crc_encoded())
  @param[in]  useWifi     True if the connection establishes via Wi-Fi. Supports high level only for now
  @param[in]  socketType  The socket flags, which are applied on socket() function call
  @param[out] conn        SbkConnection instance address to save settings to
  @return The size of the initialization message (which is just an empty control (command) message). If an error occurs, it returns -1 (check the errno)

  Example:
  @code
  ssize_t connStatus;
  SbkConnection conn = {0};
  
  connStatus = sbk_udp_open(SBK_UDP_HIGH_LEVEL_CONN, false,
                            true, SOCK_NONBLOCK, &conn)

  if (connStatus == -1)
      perror("UDP fail");
  @endcode
 */

ssize_t sbk_udp_open(const uint8_t type, const bool encode, const bool useWifi, const int socketType, SbkConnection *conn);

/**
  @brief Closes the UDP socket (thread-safe)
  @details It'a good idea to inline this function as it should be called once for every SbkConnection instance and there's usually not so many of them

  @param[in] conn The connection to close
 */

static inline __INLINE void
sbk_udp_close(SbkConnection *conn)
{
	atomic_flag *lock = &(conn->lock);

	while (atomic_flag_test_and_set_explicit(lock, memory_order_acquire))
		PAUSE;
	
	close(conn->fd);

	atomic_flag_clear_explicit(lock, memory_order_release);
}

/**
  @brief Sends a message via SbkConnection (thread-safe)  
  @details Although this function is marked with __SBK_ prefix, it's not intrinsic and is thread safe. However, it is recommended to use the sbk_udp_send macro to resolve the pointer type of the message source address
  
  @param[in]      conn SbkConnection instance
  @param[in, out] data The data to be sent, usually a control (command) message. Marked as out as the function also calculates the CRC of the message in advance
  @return The total length of sent bytes. Returns -1 on error (check the errno)
 */
ssize_t __SBK_udp_send(SbkConnection *conn, uint8_t *data);

/**
   @def Sends a message via SbkConnection (__SBK_udp_send() wrapper)
 */

#define sbk_udp_send(CONN, ADDR) \
	__SBK_udp_send(CONN, (uint8_t *) ADDR)

/**
  @brief Receives a message via SbkConnection (thread-safe)
  @details Although this function is marked with __SBK_ prefix, it's not intrinsic and is thread safe. However, it is recommended to use the sbk_udp_recv macro to resolve the pointer type of the destination buffer address
  
  @param[in]  conn SbkConnection instance
  @param[out] buf  
  @return The total length of sent bytes. Returns -1 on error (check the errno)
 */
ssize_t __SBK_udp_recv(SbkConnection *conn, void *buf);

/**
   @def Receives a message via SbkConnection (__SBK_udp_recv() wrapper)
 */

#define sbk_udp_recv(CONN, ADDR) \
	__SBK_udp_recv(CONN, (void *) ADDR)

/**
  @brief Outputs the raw bytes of the message (thread-safe)
  @details Although this function is marked with __SBK_ prefix, it's not intrinsic and is thread safe. However, it is recommended to use the sbk_print_raw_msg macro to resolve the pointer type of the message source address
  
  @param[in] bytes  Message source address
  @param[in] size   Size of the message (can be resolved through SbkConnection.size too)
 */

void __SBK_print_raw_msg(const uint8_t *, const size_t);

/**
   @def Outputs the raw bytes of the message (__SBK_print_raw_msg() wrapper)
 */

#define sbk_print_raw_msg(ADDR, SIZE) \
	__SBK_print_raw_msg((uint8_t *) ADDR, SIZE)


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
