#ifndef __SBK_CRC_H
#define __SBK_CRC_H


#include <sbk/macro.h>

#include <stdint.h>
#include <stddef.h>


/**
  @brief __SBK_gen_crc
  @details sbk_gen_crc() macro is a convenient way to use the function
  
  @param[in] msg  The control message to generate the CRC32 for
  @param[in] size The total size of the control message including the crc field
  @return The generated CRC32

  Generates the CRC32 required for the control (command) message to be accepted by the robot dog
 */

uint32_t __PURE __SBK_gen_crc(const uint32_t *msg, const size_t size);

#define sbk_gen_crc(MSG, SIZE) \
	__SBK_gen_crc((uint32_t *) MSG, SIZE)

/**
  @brief __SBK_gen_crc_encode
  @details sbk_gen_crc_encode() macro is a convenient way to use the function
  @param[in] msg  The control message to generate the encoded CRC32 for
  @param[in] size The total size of the control message including the crc field
  @return The encoded CRC32

  Generates the encoded CRC32 required for low level communication control (command) messages
 */

uint32_t __PURE __SBK_gen_crc_encode(const uint32_t *msg, const size_t size);

#define sbk_gen_crc_encode(MSG, SIZE) \
	__SBK_gen_crc_encode((uint32_t *) MSG, SIZE)


#endif // !__SBK_CRC_H
