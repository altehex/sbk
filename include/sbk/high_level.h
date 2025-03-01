/**
   @file
   @brief High Level communication function and structure definitions
  */


#ifndef __SBK_HIGH_LEVEL_H
#define __SBK_HIGH_LEVEL_H


#include <sbk/bms.h>
#include <sbk/joint.h>
#include <sbk/joystick.h>
#include <sbk/macro.h>
#include <sbk/pos.h>
#include <sbk/syncio.h>
#include <sbk/udp.h>


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
	uint8_t r, g, b, _;
} SbkLed;


typedef struct __PACKED {
	uint16_t     head;
	uint8_t      levelFlag;
	uint8_t      _r1;
	uint32_t     serialNumber[2];
	uint32_t     version[2];
	uint16_t     bandwidth;

	SbkImu       imu;
	SbkMotorHighFb  joint[20];
	SbkBmsHighFb     bms;
	
	int16_t      footForce[4];
	int16_t      _r2[4];               // footForceEst
	uint8_t      mode;
	float        _r3;                  // progress
	uint8_t      gait;
	float        footHeight;           // [m] (default = 0.08)
	float        pos[3];               // [m]
    float        bodyHeight;           // [m] (default = 0.28m)
    float        velocity[3];          // [m/s]
    float        yawSpeed;             // [rad/s]
    float        rangeObstacle[4];     // 
    SbkCoord     footPos2Body[4];      // 
    SbkCoord     footSpeed2Body[4];    // 

	SbkJoystick  wirelessRemote;

	uint32_t     _r4;

    uint32_t     crc;
} SbkHighFb;


typedef struct __PACKED {
	uint16_t     head;
	uint8_t      levelFlag;
	uint8_t      _r1;
	uint32_t     serialNumber[2];
	uint32_t     version[2];
	uint16_t     bandwidth;
	
	uint8_t      mode;
	uint8_t      gait;
	
	uint8_t      _r2; // speedLevel 
	float        footHeight;           // [m] (default = 0.08)
    float        bodyHeight;           // [m] (default = 0.28m)
	float        _r3[2];               // pos [m]
	float        euler[3];
    float        velocity[2];          // [m/s]
    float        yawSpeed;             // [rad/s]
	
	SbkBmsCtrl   bms;
	SbkLed       led;
	
	SbkJoystick  joystick;

	uint32_t     _r4[3];

    uint32_t     crc;
} SbkHighCtrl;


void __SBK_debug_print_high_fb(const SbkHighFb *);
void __SBK_debug_print_high_ctrl(const SbkHighCtrl *);

/**
   @brief Initializes the SbkHighCtrl instance

   @param[in, out] ctrl A pointer to the allocated SbkHighCtrl buffer
   @return Returns -1 if ctrl is NULL
 */

int sbk_init_high_ctrl(SbkHighCtrl *ctrl);


#endif // !__SBK_HIGH_LEVEL_H
