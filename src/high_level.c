#include <sbk/high_level.h>

#include <sbk/syncio.h>

#include <stddef.h>


void
__SBK_debug_print_high_fb(const SbkHighFb *fb)
{
	const char *fmt =
		/* "head<%zu>: %x%x\n" */
		/* "levelFlag<%zu>:\t\t%x\n" */
		/* "serialNumber<%zu>:\t\t%d-%d\n" */
		/* "imu<%zu>:\n" */
		/* "joints<%zu>:\n" */
		/* "\tRL_HIP:\n" */
		/* "\t\tq = %f\n" */
		"bms<%zu>:\n"
		"\tcurrent = %d\n"
		"footForce<%zu>\n"
		"mode<%zu>:\t\t%d\n"
		"footHeight<%zu>:\t\t%d\n"
		"bodyHeight<%zu>:\t\t%d\n";

	printf(fmt,
		   /* offsetof(SbkHighFb, head), fb->head[0], fb->head[1], */
		   /* offsetof(SbkHighFb, levelFlag), fb->levelFlag, */
		   /* offsetof(SbkHighFb, serialNumber), fb->serialNumber[0], fb->serialNumber[1], */
		   /* offsetof(SbkHighFb, imu), */
		   /* offsetof(SbkHighFb, joint), */
		   /* fb->joint[RL_HIP].q, */
		   offsetof(SbkHighFb, bms), fb->bms.current),
		   offsetof(SbkHighFb, footForce),
		   offsetof(SbkHighFb, mode), fb->mode,
		   offsetof(SbkHighFb, footHeight), fb->footHeight,
		   offsetof(SbkHighFb, bodyHeight), fb->bodyHeight;
}
