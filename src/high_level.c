#include <sbk/high_level.h>

#include <sbk/syncio.h>

#include <stddef.h>


void
__SBK_debug_print_high_fb(const SbkHighFb *fb)
{
	const char *fmt =
		"head<%zu>:         %x %x\n"
		"levelFlag<%zu>:    %d\n"
		"serialNumber<%zu>: %x %x\n"
		"imu<%zu>:\n"
		"    q: %f %f %f %f\n"
		"    g: %f %f %f\n"
		"bms<%zu>:\n"
		"footForce<%zu>:\n"
		"mode<%zu>: %d\n"
		"gait<%zu>:\n"
		"footHeight<%zu>:\n"
		"pos<%zu>:\n"
		"bodyHeight<%zu>: %f\n"
		"velocity<%zu>:\n"
		"yawSpeed<%zu>:\n"
		"rangeObstacle<%zu>:\n"
		"footPos2body<%zu>:\n"
		"footSpeed2body<%zu>:\n"
		"wirelessremote<%zu>:\n"
		"crc<%zu>:\n"
		"SbkHighFb size: %d\n"
		;

	sbk_sync_printf(fmt,
		   offsetof(SbkHighFb, head), fb->head[0], fb->head[1],
		   offsetof(SbkHighFb, levelFlag), fb->levelFlag,
		   offsetof(SbkHighFb, serialNumber), fb->serialNumber[0], fb->serialNumber[1],
		   offsetof(SbkHighFb, imu),
		       fb->imu.q[0], fb->imu.q[1], fb->imu.q[2], fb->imu.q[3], 
		       fb->imu.g[0], fb->imu.g[1], fb->imu.g[2],
		   offsetof(SbkHighFb, bms),
		   offsetof(SbkHighFb, footForce),
		   offsetof(SbkHighFb, mode), fb->mode, /* fb->mode, */
		   offsetof(SbkHighFb, gait),
		   offsetof(SbkHighFb, footHeight),
		   offsetof(SbkHighFb, pos),
		   offsetof(SbkHighFb, bodyHeight), fb->bodyHeight,
		   /* offsetof(SbkHighFb, velocity), */
		   /* offsetof(SbkHighFb, yawSpeed), */
		   /* offsetof(SbkHighFb, rangeObstacle), */
		   /* offsetof(SbkHighFb, footPos2Body), */
		   /* offsetof(SbkHighFb, footSpeed2Body), */
		   /* offsetof(SbkHighFb, wirelessRemote), */
		   /* offsetof(SbkHighFb, crc) */
		   sizeof(SbkHighFb)
		   );
}

void
__SBK_debug_print_high_ctrl(const SbkHighCtrl *ctrl)
{
	const char *fmt =
		"head<%zu>:\n"
		"levelFlag<%zu>:\n"
		"serialNumber<%zu>:\n"
		"mode<%zu>: %d\n"
		/* "gait<%zu>:\n" */
		/* "footHeight<%zu>:\n" */
		/* "bodyHeight<%zu>:\n" */
		/* "euler<%zu>:\n" */
		/* "velocity<%zu>:\n" */
		/* "yawSpeed<%zu>:\n" */
		/* "bms<%zu>:\n" */
		/* "led<%zu>:\n" */
		/* "wirelessremote<%zu>:\n" */
		/* "_r4<%zu>:\n" */
		/* "crc<%zu>:\n" */
		;

	sbk_sync_printf(fmt,
		   offsetof(SbkHighCtrl, head),
		   offsetof(SbkHighCtrl, levelFlag),
		   offsetof(SbkHighCtrl, serialNumber),
		   offsetof(SbkHighCtrl, mode), ctrl->mode/* , */
		   /* offsetof(SbkHighCtrl, gait), */
		   /* offsetof(SbkHighCtrl, footHeight), */
		   /* offsetof(SbkHighCtrl, bodyHeight), */
		   /* offsetof(SbkHighCtrl, euler), */
		   /* offsetof(SbkHighCtrl, velocity), */
		   /* offsetof(SbkHighCtrl, yawSpeed), */
		   /* offsetof(SbkHighCtrl, bms), */
		   /* offsetof(SbkHighCtrl, led), */
		   /* offsetof(SbkHighCtrl, joystick), */
		   /* offsetof(SbkHighCtrl, _r4), */
		   /* offsetof(SbkHighCtrl, crc) */
		   );
}



