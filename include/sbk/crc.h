#ifndef __SBK_CRC_H
#define __SBK_CRC_H


#include <stdint.h>
#include <stddef.h>


uint32_t __SBK_gen_crc(const uint32_t *struc, const size_t size);

#define sbk_gen_crc(STRUCT, SIZE) \
	__SBK_gen_crc(STRUCT, SIZE)

uint32_t __SBK_gen_crc_encode(const uint32_t *struc, const size_t size);

#define sbk_gen_crc_encode(STRUCT, SIZE) \
	__SBK_gen_crc_encode(STRUCT, SIZE)


#endif // !__SBK_CRC_H
