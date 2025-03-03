#include <sbk/joint.h>

#include <stddef.h>
#include <stdio.h>


void
__SBK_debug_print_motor_ctrl(SbkMotorCtrl *fb)
{
	const char *fmt =
		"mode<%zu>:\n"
		"q<%zu>:\n"
		"dq<%zu>:\n"
		"tau<%zu>:\n"
		"kp<%zu>:\n"
		"kd<%zu>:\n"
		"total size: %d\n";

	printf(fmt,
		   offsetof(SbkMotorCtrl, mode),
		   offsetof(SbkMotorCtrl, q),
		   offsetof(SbkMotorCtrl, dq),
		   offsetof(SbkMotorCtrl, tau),
		   offsetof(SbkMotorCtrl, kp),
		   offsetof(SbkMotorCtrl, kd),
		   sizeof(SbkMotorCtrl)
		   );
}


const char
*__SBK_motorFbFmt = 
			"mode<%zu>:n" 
			"q<%zu>:n" 
			"dq<%zu>:n" 
			"ddq<%zu>:n" 
			"etau<%zu>:n" 
			"t<%zu>:n" 
			"total size: %dn"; 

#define __SBK_DEFINE_print_motor_fb(LEVEL) \
	void \
    __SBK_debug_print_motor_##LEVEL##_fb(SbkMotor##LEVEL##Fb *fb) \
	{ \
		printf(__SBK_motorFbFmt, \
			   offsetof(SbkMotor##LEVEL##Fb, mode), \
			   offsetof(SbkMotor##LEVEL##Fb, q), \
			   offsetof(SbkMotor##LEVEL##Fb, dq), \
			   offsetof(SbkMotor##LEVEL##Fb, ddq), \
			   offsetof(SbkMotor##LEVEL##Fb, etau), \
			   offsetof(SbkMotor##LEVEL##Fb, t), \
			   sizeof(SbkMotor##LEVEL##Fb) \
			   ); \
	}

__SBK_DEFINE_print_motor_fb(High) 
__SBK_DEFINE_print_motor_fb(Low)

	 
uint16_t
sbk_tauf_to_tau16(const float tauf)
{
	uint16_t tauFrac, tauInt;
	
	tauFrac = (uint16_t) ((tauf - (uint16_t) tauf) * 256);
	tauInt = (uint16_t) tauf;
	
	if (tauf < 0) {
		tauInt += 255;
		tauFrac += 256;
	}

	return (tauFrac & 0xFF) | ((tauInt & 0xFF) << 8);
}


uint16_t
sbk_kpf_to_kp16(const float kpf)
{
	uint16_t kpFrac, kpInt, v;
	
	kpFrac = (uint16_t) ((kpf - (uint16_t) kpf) * 10);
	kpInt = (uint16_t) kpf;

	return (kpInt<<5) + kpFrac+kpFrac+kpFrac + (kpFrac >= 5);	
}


uint16_t
sbk_kdf_to_kd16(const float kdf)
{
	uint16_t kdInt, kdFrac, s;
	
	kdFrac = (uint16_t) ((kdf - (uint16_t) kdf) * 16);
	kdInt = (uint16_t) kdf;

	return kdFrac | (kdInt<<4);
}


void
sbk_motors_init(SbkMotorCtrl *joints)
{
	for (int j = 0; j < SBK_JOINT_COUNT; ++j) {
		joints[j].mode = SBK_MOTOR_SERVO;
		joints[j].q    = SBK_POS_STOP;
		joints[j].dq   = SBK_VEL_STOP;
	}
}
