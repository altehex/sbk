#ifndef __SBK_JOYSTICK
#define __SBK_JOYSTICK


#include <sbk/macro.h>


#define SBK_R1      1
#define SBK_L1      (1<<1)
#define SBK_START   (1<<2)
#define SBK_SELECT  (1<<3)
#define SBK_R2      (1<<4)
#define SBK_L2      (1<<5)
#define SBK_F1      (1<<6)
#define SBK_F2      (1<<7)
#define SBK_A       (1<<8)
#define SBK_B       (1<<9)
#define SBK_X       (1<<10)
#define SBK_Y       (1<<11)
#define SBK_UP      (1<<12)
#define SBK_RIGHT   (1<<13)
#define SBK_DOWN    (1<<14)
#define SBK_LEFT    (1<<15)


typedef struct __PACKED {
	uint8_t   head[2];
	uint16_t  btnMask;
	float     lx, rx, ry, L2, ly;
	uint8_t   _r[16];
} SbkJoystick;


#endif // !__SBK_JOYSTICK
