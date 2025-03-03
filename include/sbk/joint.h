#ifndef __SBK_JOINT_H
#define __SBK_JOINT_H


#include <sbk/macro.h>

#include <stdint.h>


#define SBK_HIP_MAX     (1.047f)
#define SBK_HIP_MIN     (-1.047f)
#define SBK_CALF_MAX    (-0.837f)
#define SBK_CALF_MIN    (-2.721f)
#define SBK_THIGH_MAX   (2.966f)
#define SBK_THIGH_MIN   (-0.663f)

#define SBK_POS_STOP  (2.146E+9f)
#define SBK_VEL_STOP  (16000.f)

#define SBK_MOTOR_SERVO     0x0A
#define SBK_MOTOR_DAMPING   0x00
#define SBK_MOTOR_OVERHEAT  0x08


typedef enum {
	FR, FL, RR, RL
} SbkLeg;

typedef enum {
	HIP, THIGH, CALF
} SbkJointIndex;

typedef enum {
	FR_HIP, FR_THIGH, FR_CALF,
	FL_HIP, FL_THIGH, FL_CALF,
	RR_HIP, RR_THIGH, RR_CALF,
	RL_HIP, RL_THIGH, RL_CALF,
	SBK_JOINT_COUNT
} SbkJoint;


typedef struct __PACKED {
	uint8_t  mode;   // motor mode (see SBK_MOTOR_ constants)
	float    q;      // angle [rad]
	float    dq;     // velocity [rad/s]
	float    ddq;    // acceleration [rad/s^2] 
	float    etau;   // extimated torque [N*m]
	float    _q, _dq;
	float    _ddq; // reserved
	int8_t   t;      // temperature
	uint32_t _r[2];  // reserved
} SbkMotorHighFb;


typedef struct __PACKED {
	uint8_t   mode;   // motor mode (see SBK_MOTOR_ constants)
	float     q;      // angle [rad]
	float     dq;     // velocity [rad/s]
	uint16_t  ddq;    // acceleration [rad/s^2] 
	uint16_t  etau;   // extimated torque [N*m]
	float     _q, _dq;
	uint16_t  _ddq; // reserved
	int8_t    t;      // temperature
	uint32_t  _r[2];  // reserved
} SbkMotorLowFb;


typedef struct __PACKED {
	uint8_t  mode;   // motor mode (see SBK_MOTOR_ constants)
	float    q;      // angle [rad]
	float    dq;     // velocity [rad/s]
	uint16_t tau;    // torque [N*m]
	uint16_t    kp;  // position stiffness [N*m/rad]
	uint16_t    kd;   // velocity stiffness [N*m/(rad/s)]
	uint32_t _r[3];  // reserved
} SbkMotorCtrl;


/**
   @brief Converts the 32-bit float tau (torque) value to 16-bit float

   @param[in] tauf Torque of float type
   @return Torque of uint16_t type
 */

__PURE uint16_t sbk_tauf_to_tau16(const float tauf);

/**
   @brief Converts the 32-bit float kp (position stiffness) value to 16-bit float

   @param[in] kpf Position stiffness of float type
   @return Position stiffness of uint16_t type
 */

__PURE uint16_t sbk_kpf_to_kp16(const float kpf);

/**
   @brief Converts the 32-bit float kd (velocity stiffness) value to 16-bit float

   @param[in] kdf Velocity stiffness of float type
   @return Velocity stiffness of uint16_t type
 */

__PURE uint16_t sbk_kdf_to_kd16(const float kdf);

/**
   @brief Motor cotnrol structure initialization

   @param[out] joints Array of 12 motor control structures
 */
void sbk_motors_init(SbkMotorCtrl *joints);


#ifdef DEBUG

void __SBK_debug_print_motor_ctrl(SbkMotorCtrl *);
void __SBK_debug_print_motor_High_fb(SbkMotorHighFb *);
void __SBK_debug_print_motor_Low_fb(SbkMotorLowFb *);

#define __SBK_debug_print_motor_low_fb(FB) \
	__SBK_debug_print_motor_Low_fb(FB)

#define __SBK_debug_print_motor_high_fb(FB) \
	__SBK_debug_print_motor_High_fb(FB)

#endif // !DEBUG


#endif // !__SBK_JOINT_H
