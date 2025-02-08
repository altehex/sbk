#ifndef __SBK_LOW_LEVEL_H
#define __SBK_LOW_LEVEL_H

#define pragma pack(1)


#include <sbk/bms.h>
#include <sbk/joint.h>
#include <sbk/pos.h>

#include <stdint.h>


#define SBK_LOW_LEVEL 0xFF


typedef struct {
	uint8_t    head[2];
	uint8_t    levelFlag;
	uint8_t    _frameReserve;
	uint32_t   serialNumber[2];
	uint32_t   version[2];
	uint16_t   bandwidth;
	
	SbkImu     imu;
	SbkMotorFb joint[20];
	SbkBmsFb   bms;

	int16_t    footForce[4];

	int16_t    _r1[4];

	uint32_t   tick; // [ms]

	uint8_t    remote[40];

	uint32_t   _r2;

	//uint32_t   crc;
} SbkLowFb;


typedef struct {
	uint8_t      head[2];
	uint8_t      levelFlag;
	uint8_t      _frameReserve;
	uint32_t     serialNumber[2];
	uint32_t     version[2];
	uint16_t     bandwidth;

	SbkMotorCtrl joint[20];
	SbkBmsCtrl   bms;

	uint8_t      _r1[40];
	uint32_t     _r2;

	uint32_t     crc;
} SbkLowCtrl;


uint16_t to_tau(float);
uint16_t to_kp(float);
uint16_t to_kd(float);


#endif // !__SBK_LOW_LEVEL_H
