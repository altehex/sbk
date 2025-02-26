#ifndef __SBK_LOW_LEVEL_H
#define __SBK_LOW_LEVEL_H


#include <sbk/bms.h>
#include <sbk/joint.h>
#include <sbk/joystick.h>
#include <sbk/macro.h>
#include <sbk/pos.h>

#include <math.h>
#include <stdint.h>


#define SBK_LOW_LEVEL 0xFF


typedef struct __PACKED {
	uint8_t      head[2];
	uint8_t      levelFlag;
	uint8_t      _frameReserve;
	uint32_t     serialNumber[2];
	uint32_t     version[2];
	uint16_t     bandwidth;
	
	SbkImu       imu;
	SbkMotorLowFb   joint[20];
	SbkBmsFb     bms;

	int16_t      footForce[4];

	int16_t      _r1[4];

	uint32_t     tick; // [ms]

	SbkJoystick  joystick;

	uint32_t     _r2;

	//uint32_t   crc;
} SbkLowFb;


typedef struct __PACKED {
	uint8_t      head[2];
	uint8_t      levelFlag;
	uint8_t      _frameReserve;
	uint32_t     serialNumber[2];
	uint32_t     version[2];
	uint16_t     bandwidth;

	SbkMotorCtrl joint[20];
	SbkBmsCtrl   bms;

	SbkJoystick  joystick;
	uint32_t     _r1;

	uint32_t     crc;
} SbkLowCtrl;


/* static inline uint16_t __PURE */
/* to_tau16(const float ftau) */
/* { */
/* 	uint16_t f, i; */

/* 	i = (uint16_t) round(ftau); */
/* 	f = ((uint16_t) (ftau-(float) i))<<8; */

/* 	if (ftau < 0) { */
/* 		i += 255; */
/* 		f += 256; */
/* 	} */
	
/* 	return i + f; */
/* } */


/* static inline uint16_t __PURE */
/* to_kp16(const float fkp) */
/* { */
/* 	uint16_t f, i, v; */

/* 	i = (uint16_t) round(fkp); */
/* 	f = (uint16_t) (fkp-(float) i); */
/* 	f = f<<3 + f<<1; // f *= 10 */

/* 	v = f % ( f < 5 */
/* 	        ? (i << 5) + 3*f */
/* 	        : (i << 5) + 3*(f-1) + 4 ); */

/* 	return ((v & 0xFF) << 8) | ((v & 0xFF00) >> 8); */
/* } */


/* static inline uint16_t __PURE */
/* to_kd16(const float fkd) */
/* { */
/* 	uint16_t f, i; */

/* 	i = (uint16_t) round(fkd); */
/* 	f = (uint16_t) (fkp-(float) i); */
/* } */


void __SBK_debug_print_low_fb(const SbkLowFb *);
void __SBK_debug_print_low_ctrl(const SbkLowCtrl *);


#endif // !__SBK_LOW_LEVEL_H
