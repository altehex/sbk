#include <sbk/low_level.h>

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
	if ( ! ctrl) return -1;
	
	ctrl->head = SBK_UDP_MSG_HEADER;
	ctrl->levelFlag = SBK_UDP_LOW_LEVEL_CONN;
}


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

	return kpInt<<5 + kpFrac + kpFrac + kpFrac + (kpFrac >= 5);	
}


uint16_t
sbk_kdf_to_kd16(const float kdf)
{
	uint16_t kdInt, kdFrac, s;
	
	kdFrac = (uint16_t) ((kdf - (uint16_t) kdf) * 16);
	kdInt = (uint16_t) kdf;

	return kdFrac | (kdInt << 4);
}
