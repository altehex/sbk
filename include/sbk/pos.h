#ifndef __SBK_POS_H
#define __SBK_POS_H

#pragma pack(1)


#include <stdint.h>


// Do not change the fields as this structure is used
// for UDP message passing

typedef struct {
	float  q[4]; // quaternion
	float  g[3]; // gyroscope
	float  a[3]; // accelerometer
	float  r[3]; // euler angle
	int8_t t;    // IMU temperature
} SbkImu;


typedef struct {
	float x, y, z;
} SbkCoord;


#endif // !__SBK_POS_H
