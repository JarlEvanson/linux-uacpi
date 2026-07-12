#pragma once

#include <uacpi/kernel_api.h>
#include <uacpi/log.h>

/* logging interface and implementation adapted from uacpi/internal/log.h */
bool shim_should_log(uacpi_log_level level);

#define shim_log(level, fmt, ...)                                      \
	do {                                                           \
		if (shim_should_log(level)) {                          \
			uacpi_kernel_log(level,                        \
					 UACPI_START_OF_LOG_MSG fmt    \
						 UACPI_END_OF_LOG_MSG, \
					 ##__VA_ARGS__);               \
		}                                                      \
	} while (0)

#define shim_debug(...) shim_log(UACPI_LOG_DEBUG, __VA_ARGS__)
#define shim_trace(...) shim_log(UACPI_LOG_TRACE, __VA_ARGS__)
#define shim_info(...) shim_log(UACPI_LOG_INFO, __VA_ARGS__)
#define shim_warn(...) shim_log(UACPI_LOG_WARN, __VA_ARGS__)
#define shim_error(...) shim_log(UACPI_LOG_ERROR, __VA_ARGS__)
