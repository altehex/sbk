#include <bms.h>


#include <sbk/syncio.h>

#include <stddef.h>


void
__SBK_debug_print_bms_fb(const SbkBmsFb *fb)
{
	const char *fmt =
		"version<%zu>: %x %x\n"
		"status<%zu>: \n"
		"soc<%zu>: \n"
		"current<%zu>: \n"
		"cycle<%zu>: \n"
		"bqNtc<%zu>: \n"
		"mcuNtc<%zu>: \n"
		"voltage<%zu>: \n";
	
	sbk_printf(fmt);
}
