#ifndef __SBK_HIGH_LEVEL_H
#define __SBK_HIGH_LEVEL_H


#include <sbk/bms.h>
#include <sbk/joint.h>
#include <sbk/macro.h>
#include <sbk/pos.h>
#include <sbk/udp.h>


#define SBK_HIGH_LEVEL_UDP_PORT 8082


typedef enum {
	SBK_GAIT_IDLE,
	SBK_GAIT_TROT,
	SBK_GAIT_TROT_RUN,
	SBK_GAIT_CLIMB_STAIRS,
	SBK_GAIT_TROT_OBSTACLE
} SbkGait;


typedef enum {
	SBK_MODE_IDLE,
	SBK_MODE_FORCE_STAND,
	SBK_MODE_TARGET_VELOCITY,
	SBK_MODE_TARGET_POS,
	SBK_MODE_PATH,
	SBK_MODE_STAND_DOWN,
	SBK_MODE_STAND_UP,
	SBK_MODE_DAMPING,
	SBK_MODE_RECOVERY,
	SBK_MODE_BACKFLIP,
	SBK_MODE_JUMP_YAW,
	SBK_MODE_STRAIGHT_HAND
} SbkMode;


typedef struct __PACKED {
	uint8_t r, g, b;
} SbkLed;


typedef struct __PACKED {
	uint8_t    head[2];
	uint8_t    levelFlag;
	uint8_t    _r1;
	uint32_t   serialNumber[2];
	uint32_t   version[2];
	uint16_t   bandwidth;

	SbkImu     imu;
	SbkMotorFb joint[20];
	SbkBmsFb   bms;
	
	int16_t    footForce[4];
	int16_t    _r2[4];               // footForceEst
	uint8_t    mode;
	float      _r3;                  // progress
	uint8_t    gait;
	float      footHeight;           // [m] (default = 0.08)
	float      pos[3];               // [m]
    float      bodyHeight;           // [m] (default = 0.28m)
    float      velocity[3];          // [m/s]
    float      yawSpeed;             // [rad/s]
    float      rangeObstacle[4];     // 
    SbkCoord   footPos2Body[4];      // 
    SbkCoord   footSpeed2Body[4];    // 

	uint8_t    wirelessRemote[40];

	uint32_t   _r4;

    uint32_t   crc;
} SbkHighFb;


typedef struct __PACKED {
	uint8_t    head[2];
	uint8_t    levelFlag;
	uint8_t    _r1;
	uint32_t   serialNumber[2];
	uint32_t   version[2];
	uint16_t   bandwidth;
	
	uint8_t    mode;
	uint8_t    gait;
	
	uint8_t    _r2; // speedLevel 
	float      footHeight;           // [m] (default = 0.08)
    float      bodyHeight;           // [m] (default = 0.28m)
	float      _r3[2];               // pos [m]
	float      euler[3];
    float      velocity[2];          // [m/s]
    float      yawSpeed;             // [rad/s]
	
	SbkBmsCtrl bms;
	SbkLed     led[4];
	
	uint8_t    wirelessRemote[40];

	uint32_t   _3;

    uint32_t   crc;
} SbkHighCtrl;


#endif // !__SBK_HIGH_LEVEL_H
