#include <sbk/crc.h>

#include <stddef.h>


#include <stdio.h>

uint32_t
sbk_gen_crc(const uint32_t  *struc,
			const size_t   sz)
{
	uint32_t crc, x;

	crc = 0xFFFFFFFF;

	for (int i = 0; i < (sz-4)>>2; ++i)
		for (int b = 0; b < 32; ++b) {
			x = (crc>>31) & 1;
			crc <<= 1;
			crc &= 0xFFFFFFFF;
			if (x ^ (1 & (struc[i] >> (31-b))))
				crc ^= 0x04c11db7;
		}

	printf("CRC: %x\n", crc);
	
	return crc;
}
