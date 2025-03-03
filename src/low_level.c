#include <sbk/low_level.h>

#include <sbk/joint.h>
#include <sbk/macro.h>
#include <sbk/syncio.h>
#include <sbk/udp.h>

#include <math.h>
#include <stddef.h>


void
__SBK_debug_print_low_fb(const SbkLowFb *fb)
{
	const char *fmt =
		"head<%zu>: %x\n"
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
		   offsetof(SbkLowFb, head), fb->head,
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


int
sbk_init_low_ctrl(SbkLowCtrl *ctrl)
{
	SbkMotorCtrl *joints;
	
	if ( ! ctrl) return -1;
	
	ctrl->head = SBK_UDP_MSG_HEADER;
	ctrl->levelFlag = SBK_UDP_LOW_LEVEL_CONN;

	sbk_motors_init(ctrl->joint);
}

