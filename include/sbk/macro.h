#ifndef __SBK_MACRO_H
#define __SBK_MACRO_H


#define __PACKED __attribute__((packed))
#define __PURE __attribute__((pure))

#define __MIN(A, B) \
	({ __typeof__(A) ____MINA = (A), ____MINB = (B); \
	   ____MINA < ____MINB ? ____MINA : ____MINB; })

#define __MAX(A, B) \
	({ __typeof__(A) ____MAXA = (A), ____MAXB = (B); \
	   ____MAXA < ____MAXB ? ____MAXA : ____MAXB; })


#endif // !__SBK_MACRO_H
