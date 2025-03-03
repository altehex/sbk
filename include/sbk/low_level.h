/**
   @file
   @brief Low Level communication function and structure definitions
  */

#ifndef __SBK_LOW_LEVEL_H
#define __SBK_LOW_LEVEL_H


#include <sbk/bms.h>
#include <sbk/joint.h>
#include <sbk/joystick.h>
#include <sbk/macro.h>
#include <sbk/pos.h>

#include <stdint.h>


typedef struct __PACKED {
	uint16_t     head;
	uint8_t      levelFlag;
	uint8_t      _frameReserve;
	uint32_t     serialNumber[2];
	uint32_t     version[2];
	uint16_t     bandwidth;
	
	SbkImu        imu;
	SbkMotorLowFb joint[20];
	SbkBmsLowFb   bms;

	int16_t      footForce[4];

	int16_t      _r1[4];

	uint32_t     tick; // [ms]

	SbkJoystick  joystick;

	uint32_t     _r2;

	uint32_t   crc;
} SbkLowFb;


typedef struct __PACKED {
	uint16_t     head;
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


/**
   @brief Converts the 32-bit float tau (torque) value to 16-bit float

   @param[in] tauf Torque of float type
   @return Torque of uint16_t type
 */

__PURE uint16_t sbk_tauf_to_tau16(const float tauf);

/**
   @brief Torque (tau) setter for low level control message

   @param[in]  torquef Torque of float type
   @param[in]  joint   Joint index (see SbkJoint in joint.h)
   @param[out] ctrl    Control message where tau is set
  */

static inline void
sbk_set_torque(const float   torquef,
		       SbkJoint      joint,
		       SbkLowCtrl    *ctrl)
{
	ctrl->joint[joint].tau = sbk_tauf_to_tau16(torquef);
}

/**
   @brief Converts the 32-bit float kp (position stiffness) value to 16-bit float

   @param[in] kpf Position stiffness of float type
   @return Position stiffness of uint16_t type
 */

__PURE uint16_t sbk_kpf_to_kp16(const float kpf);

/**
   @brief Position stiffness (kp) setter for low level control message

   @param[in]  kpf   Position stiffness of float type
   @param[in]  joint Joint index (see SbkJoint in joint.h)
   @param[out] ctrl  Control message where kp is set
  */

static inline void
sbk_set_kp(const float   kpf,
	       SbkJoint      joint,
	       SbkLowCtrl    *ctrl)
{
	ctrl->joint[joint].kp = sbk_kpf_to_kp16(kpf);
}

/**
   @brief Converts the 32-bit float kd (velocity stiffness) value to 16-bit float

   @param[in] kdf Velocity stiffness of float type
   @return Velocity stiffness of uint16_t type
 */

__PURE uint16_t sbk_kdf_to_kd16(const float kdf);

/**
   @brief Velocity stiffness (kd) setter for low level control message

   @param[in]  kdf   Velocity stiffness of float type
   @param[in]  joint Joint index (see SbkJoint in joint.h)
   @param[out] ctrl  Control message where kd is set
  */

static inline void
sbk_set_kd(const float   kdf,
	       SbkJoint      joint,
	       SbkLowCtrl    *ctrl)
{
	ctrl->joint[joint].kd = sbk_kdf_to_kd16(kdf);
}


void __SBK_debug_print_low_fb(const SbkLowFb *);
void __SBK_debug_print_low_ctrl(const SbkLowCtrl *);

/**
   @brief Initializes the SbkLowCtrl instance

   @param[in, out] ctrl A pointer to the allocated SbkLowCtrl buffer
   @return Returns -1 if ctrl is NULL
 */

int sbk_init_low_ctrl(SbkLowCtrl *ctrl);


#endif // !__SBK_LOW_LEVEL_H
