#pragma once

#include <linux/acpi.h>

#include <uacpi/kernel_api.h>
#include <uacpi/log.h>
#include <uacpi/status.h>

void shim_tables_initialize(void);
void shim_tables_deinitialize(void);

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

static inline acpi_status uacpi_to_acpi_status(uacpi_status status)
{
	switch (status) {
	case UACPI_STATUS_OK:
		return AE_OK;
	case UACPI_STATUS_MAPPING_FAILED:
		return AE_NO_MEMORY;
	case UACPI_STATUS_OUT_OF_MEMORY:
		return AE_NO_MEMORY;
	case UACPI_STATUS_BAD_CHECKSUM:
		return AE_BAD_CHECKSUM;
	case UACPI_STATUS_INVALID_SIGNATURE:
		return AE_BAD_SIGNATURE;
	case UACPI_STATUS_INVALID_TABLE_LENGTH:
		return AE_BAD_HEADER;
	case UACPI_STATUS_NOT_FOUND:
		return AE_NOT_FOUND;
	case UACPI_STATUS_INVALID_ARGUMENT:
		return AE_BAD_PARAMETER;
	case UACPI_STATUS_UNIMPLEMENTED:
		return AE_NOT_IMPLEMENTED;
	case UACPI_STATUS_ALREADY_EXISTS:
		return AE_ALREADY_EXISTS;
	case UACPI_STATUS_INTERNAL_ERROR:
		return AE_ERROR;
	case UACPI_STATUS_TYPE_MISMATCH:
		return AE_TYPE;
	case UACPI_STATUS_INIT_LEVEL_MISMATCH:
		return AE_BAD_PARAMETER;
	case UACPI_STATUS_NAMESPACE_NODE_DANGLING:
		return AE_NULL_ENTRY;
	case UACPI_STATUS_NO_HANDLER:
		return AE_NOT_EXIST;
	case UACPI_STATUS_NO_RESOURCE_END_TAG:
		return AE_AML_NO_RESOURCE_END_TAG;
	case UACPI_STATUS_COMPILED_OUT:
		return AE_SUPPORT;
	case UACPI_STATUS_HARDWARE_TIMEOUT:
	case UACPI_STATUS_TIMEOUT:
		return AE_TIME;
	case UACPI_STATUS_OVERRIDDEN:
		return AE_OK;
	case UACPI_STATUS_DENIED:
		return AE_ACCESS;
	default:
		if (status >= UACPI_STATUS_AML_UNDEFINED_REFERENCE)
			return AE_AML_INTERNAL;
		return AE_ERROR;
	}
}
