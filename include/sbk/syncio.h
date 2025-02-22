#ifndef __SBK_SYNCIO_H
#define __SBK_SYNCIO_H


#include <stdarg.h>
#include <stdatomic.h>
#include <stdio.h>


extern atomic_flag __SBK_stdoutLock;


typedef enum {
	NO_SYNC,
	SYNC
} SbkEnableSync;


__attribute__((format (printf, 1, 2)))
static inline int
sbk_sync_printf(const char *fmt, ...)
{
	va_list args;

	if ( ! atomic_flag_test_and_set_explicit(&__SBK_stdoutLock,
											 memory_order_acquire)) {
		va_start(args, fmt);
		printf(fmt, args);

		atomic_flag_clear_explicit(&__SBK_stdoutLock,
								   memory_order_release);
		va_end(args);
	}
}


static inline void
sbk_sync_puts(const char *str)
{
	if ( ! atomic_flag_test_and_set_explicit(&__SBK_stdoutLock,
											 memory_order_acquire)) {
		puts(str);
		atomic_flag_clear_explicit(&__SBK_stdoutLock,
								   memory_order_release);
	}
}


#endif // !__SBK_SYNCIO_H
