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
