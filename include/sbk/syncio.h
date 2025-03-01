#ifndef __SBK_SYNCIO_H
#define __SBK_SYNCIO_H


#include <stdarg.h>
#include <stdatomic.h>
#include <stdio.h>


extern atomic_flag __SBK_stdoutLock;


#define sbk_sync_printf(FMT, ...) \
	if ( ! atomic_flag_test_and_set_explicit(&__SBK_stdoutLock, \
											 memory_order_acquire)) { \
		printf(FMT, ##__VA_ARGS__); \
		atomic_flag_clear_explicit(&__SBK_stdoutLock, \
								   memory_order_release); \
	}


/**
  @brief sbk_sync_puts
  @param[in] str The string to output
  
  A thread safe puts() wrapper
 */

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
