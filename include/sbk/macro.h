#ifndef __SBK_MACRO_H
#define __SBK_MACRO_H


#define __INLINE __attribute__((always_inline))
#define __PACKED __attribute__((packed))
#define __PURE __attribute__((pure))


#define __MIN(A, B)										\
	({ __typeof__(A) ____MINA = (A), ____MINB = (B);	\
	   ____MINA < ____MINB ? ____MINA : ____MINB; })

#define __MAX(A, B) \
	({ __typeof__(A) ____MAXA = (A), ____MAXB = (B);	\
	   ____MAXA < ____MAXB ? ____MAXA : ____MAXB; })


#ifdef ARCH_X86
#    define PAUSE \
	__asm__ __volatile__ ( "pause" )
#else
#    define PAUSE continue
#endif


#endif // !__SBK_MACRO_H
