#include <sbk/low_level.h>

#include <sbk/syncio.h>


void
__SBK_debug_print_low_fb(const SbkLowFb *fb)
{
	const char *fmt =
		"head: %x%x\n"
		"levelFlag:\t\t%x\n"
		"serialNumber:\t\t%d-%d\n"
		"joints:\n"
		"\tRL_HIP:\n"
		"\t\tq = %f\n"
		"bms:\n"
		"\tcurrent = %d\n";

	printf(fmt,
		   fb->head[0], fb->head[1],
		   fb->levelFlag,
		   fb->serialNumber[0], fb->serialNumber[1],
		   fb->joint[RL_HIP].q,
		   fb->bms.current);
}
