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


void
__SBK_debug_print_motor_fb(SbkMotorHighFb *fb)
{
	const char *fmt =
		"mode<%zu>:\n"
		"q<%zu>:\n"
		"dq<%zu>:\n"
		"ddq<%zu>:\n"
		"etau<%zu>:\n"
		"t<%zu>:\n"
		"total size: %d\n";

	printf(fmt,
		   offsetof(SbkMotorHighFb, mode),
		   offsetof(SbkMotorHighFb, q),
		   offsetof(SbkMotorHighFb, dq),
		   offsetof(SbkMotorHighFb, ddq),
		   offsetof(SbkMotorHighFb, etau),
		   offsetof(SbkMotorHighFb, t),
		   sizeof(SbkMotorHighFb)
		   );
}
