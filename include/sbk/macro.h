#ifndef __SBK_MACRO_H
#define __SBK_MACRO_H


#define __PACKED __attribute__((packed))

#define __MIN(A, B) \
	({ __typeof__(A) ____A = (A), ____B = (B); \ 
	   ____A < ____B ? ____A : ____B; })


#endif // !__SBK_MACRO_H
