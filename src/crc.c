#include <sbk/crc.h>

#include <sbk/macro.h>

#include <stddef.h>


uint32_t
__SBK_gen_crc(const uint32_t  *msg,
			  const size_t    sz)
{
	uint32_t crc, x;

	crc = 0xFFFFFFFF;

	for (int i = 0; i < (sz-4)>>2; ++i)
		for (int b = 0; b < 32; ++b) {
			x = (crc>>31) & 1;
			crc <<= 1;
			crc &= 0xFFFFFFFF;
			if (x ^ (1 & (msg[i] >> (31-b))))
				crc ^= 0x04C11DB7;
		}

	return crc;
}


uint32_t
__SBK_gen_crc_encode(const uint32_t *msg,
					 const size_t   sz)
{
	uint32_t crc = __SBK_gen_crc(msg, sz) ^ 0xEDCAB9DE;
	return ROR(crc, 8);
}
