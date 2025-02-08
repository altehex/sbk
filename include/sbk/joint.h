#ifndef __SBK_JOINT_H
#define __SBK_JOINT_H


#include <sbk/macro.h>

#include <stdint.h>


#define SBK_MOTOR_SERVO    0x0A
#define SBK_MOTOR_DAMPING  0x00
#define SBK_MOTOR_OVERHEAT 0x08


typedef enum {
	FR, FL, RR, RL
} SbkLeg;

typedef enum {
	FR_HIP, FR_THIGH, FR_CALF,
	FL_HIP, FL_THIGH, FL_CALF,
	RR_HIP, RR_THIGH, RR_CALF,
	RL_HIP, RL_THIGH, RL_CALF
} SbkJoint;


typedef struct __PACKED {
	uint8_t  mode;   // motor mode (see SBK_MOTOR_ constants)
	float    q;      // angle [rad]
	float    dq;     // velocity [rad/s]
	float    ddq;    // acceleration [rad/s^2] 
	float    etau;   // extimated torque [N*m]
	float    _q, _dq, _ddq; // reserved
	int8_t   t;      // temperature
	uint32_t _r[3];  // reserved
} SbkMotorFb;


typedef struct __PACKED {
	uint8_t  mode;   // motor mode (see SBK_MOTOR_ constants)
	float    q;      // angle [rad]
	float    dq;     // velocity [rad/s]
	uint16_t tau;    // torque [N*m]
	uint16_t    kp;  // position stiffness [N*m/rad]
	uint16_t    kd;   // velocity stiffness [N*m/(rad/s)]
	uint32_t _r[3];  // reserved
} SbkMotorCtrl;


#endif // !__SBK_JOINT_H
