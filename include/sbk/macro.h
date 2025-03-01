#ifndef __SBK_MACRO_H
#define __SBK_MACRO_H


#include <stdint.h>


#define __INLINE __attribute__((always_inline))
#define __PACKED __attribute__((packed))
#define __PURE __attribute__((pure))


#define __MIN(A, B)										\
	({ __typeof__(A) ____MINA = (A), ____MINB = (B);	\
	   ____MINA < ____MINB ? ____MINA : ____MINB; })

#define __MAX(A, B) \
	({ __typeof__(A) ____MAXA = (A), ____MAXB = (B);	\
	   ____MAXA < ____MAXB ? ____MAXA : ____MAXB; })


#if defined (__x86_64__) || defined (ARCH_X86)
#    define PAUSE								\
	__asm__ __volatile__ ( "pause" )
#else
#    define PAUSE continue
#endif


#define ROR(X, R)								\
	(X >> R) | (X << (32-R))	  


#if defined (ARCH_X86)

#   define SWAP16(X) __builtin_bswap16(X)

#elif defined (__ARM_ARCH_7A__) || defined (__ARM_ARCH_7__)

static inline uint32_t
REV16(uint32_t x)
{
	register uint32_t y;

	__asm__ __volatile__ ("rev16 %0, %1" : "=r"(y) : "y"(x));

	return y;
}

#   define SWAP16(X) (uint16_t) REV16((uint32_t) X)
	
#else

#   define SWAP16(X) ((X & 0xFF) << 8) | ((X & 0xFF00) >> 8)

#endif // SWAP16


#endif // !__SBK_MACRO_H
