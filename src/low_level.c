#include <sbk/low_level.h>

#include <sbk/syncio.h>

#include <stddef.h>


void
__SBK_debug_print_low_fb(const SbkLowFb *fb)
{
	const char *fmt =
		"head<%zu>: %x %x\n"
		"levelFlag<%zu>: %d\n"
		"serialNumber<%zu>: %d %d\n"
		"imu<%zu>:\n"
		"\tq: %f %f %f %f\n"
		"\tg: %f %f %f\n"
		"joints<%zu>:\n"
		"bms<%zu>:\n"
		"footForce<%zu>:\n"
		"tick<%zu>:\n"
		"joystick<%zu>:\n";

	printf(fmt,
		   offsetof(SbkLowFb, head), fb->head[0], fb->head[1],
		   offsetof(SbkLowFb, levelFlag), fb->levelFlag,
		   offsetof(SbkLowFb, serialNumber), fb->serialNumber[0], fb->serialNumber[1],
		   offsetof(SbkLowFb, imu),
		       fb->imu.q[0], fb->imu.q[1], fb->imu.q[2], fb->imu.q[3], 
		       fb->imu.g[0], fb->imu.g[1], fb->imu.g[2],
		   offsetof(SbkLowFb, joint),
		   offsetof(SbkLowFb, bms),
		   offsetof(SbkLowFb, footForce),
		   offsetof(SbkLowFb, tick),
		   offsetof(SbkLowFb, joystick)
		   );
}


void
__SBK_debug_print_low_ctrl(const SbkLowCtrl *ctrl)
{
	const char *fmt =
		"head<%zu>:\n"
		"levelFlag<%zu>:\n"
		"serialNumber<%zu>:\n"
		"joints<%zu>:\n"
		"bms<%zu>:\n";

	printf(fmt,
		   offsetof(SbkLowCtrl, head),
		   offsetof(SbkLowCtrl, levelFlag),
		   offsetof(SbkLowCtrl, serialNumber),
		   offsetof(SbkLowCtrl, joint),
		   offsetof(SbkLowCtrl, bms)
		   );
}
