#pragma once

#include <linux/acpi.h>

#include <uacpi/kernel_api.h>
#include <uacpi/log.h>
#include <uacpi/namespace.h>
#include <uacpi/status.h>

uacpi_status shim_namespace_initialize(void);
void shim_namespace_deinitialize(void);

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

static inline uacpi_namespace_node *uacpi_node_from_handle(acpi_handle handle)
{
	if (!handle || handle == ACPI_ROOT_OBJECT)
		return uacpi_namespace_root();

	return handle;
}

static inline acpi_handle acpi_handle_from_node(uacpi_namespace_node *node)
{
	if (!node || node == uacpi_namespace_root())
		return ACPI_ROOT_OBJECT;

	return node;
}

static inline acpi_status uacpi_to_acpi_type(uacpi_object_type in,
					     acpi_object_type *out)
{
	if (!out)
		return AE_BAD_PARAMETER;

	switch (in) {
	case UACPI_OBJECT_UNINITIALIZED:
		*out = ACPI_TYPE_ANY;
		break;
	case UACPI_OBJECT_INTEGER:
		*out = ACPI_TYPE_INTEGER;
		break;
	case UACPI_OBJECT_STRING:
		*out = ACPI_TYPE_STRING;
		break;
	case UACPI_OBJECT_BUFFER:
		*out = ACPI_TYPE_BUFFER;
		break;
	case UACPI_OBJECT_PACKAGE:
		*out = ACPI_TYPE_PACKAGE;
		break;
	case UACPI_OBJECT_FIELD_UNIT:
		*out = ACPI_TYPE_FIELD_UNIT;
		break;
	case UACPI_OBJECT_DEVICE:
		*out = ACPI_TYPE_DEVICE;
		break;
	case UACPI_OBJECT_EVENT:
		*out = ACPI_TYPE_EVENT;
		break;
	case UACPI_OBJECT_METHOD:
		*out = ACPI_TYPE_METHOD;
		break;
	case UACPI_OBJECT_MUTEX:
		*out = ACPI_TYPE_MUTEX;
		break;
	case UACPI_OBJECT_OPERATION_REGION:
		*out = ACPI_TYPE_REGION;
		break;
	case UACPI_OBJECT_POWER_RESOURCE:
		*out = ACPI_TYPE_POWER;
		break;
	case UACPI_OBJECT_PROCESSOR:
		*out = ACPI_TYPE_PROCESSOR;
		break;
	case UACPI_OBJECT_THERMAL_ZONE:
		*out = ACPI_TYPE_THERMAL;
		break;
	case UACPI_OBJECT_BUFFER_FIELD:
		*out = ACPI_TYPE_BUFFER_FIELD;
		break;
	case UACPI_OBJECT_DEBUG:
		*out = ACPI_TYPE_DEBUG_OBJECT;
		break;
	default:
		shim_error("unrecognized type %s given to uacpi_to_acpi_type",
			   uacpi_object_type_to_string(in));
		return AE_NOT_IMPLEMENTED;
	}

	return AE_OK;
}

static inline acpi_status
acpi_type_to_uacpi_type_bits(acpi_object_type in, uacpi_object_type_bits *out)
{
	if (!out)
		return AE_BAD_PARAMETER;

	switch (in) {
	case ACPI_TYPE_ANY:
		*out = UACPI_OBJECT_ANY_BIT;
		break;
	case ACPI_TYPE_INTEGER:
		*out = UACPI_OBJECT_INTEGER_BIT;
		break;
	case ACPI_TYPE_STRING:
		*out = UACPI_OBJECT_STRING_BIT;
		break;
	case ACPI_TYPE_BUFFER:
		*out = UACPI_OBJECT_BUFFER_BIT;
		break;
	case ACPI_TYPE_PACKAGE:
		*out = UACPI_OBJECT_PACKAGE_BIT;
		break;
	case ACPI_TYPE_FIELD_UNIT:
		*out = UACPI_OBJECT_FIELD_UNIT_BIT;
		break;
	case ACPI_TYPE_DEVICE:
		*out = UACPI_OBJECT_DEVICE_BIT;
		break;
	case ACPI_TYPE_EVENT:
		*out = UACPI_OBJECT_EVENT_BIT;
		break;
	case ACPI_TYPE_METHOD:
		*out = UACPI_OBJECT_METHOD_BIT;
		break;
	case ACPI_TYPE_MUTEX:
		*out = UACPI_OBJECT_MUTEX_BIT;
		break;
	case ACPI_TYPE_REGION:
		*out = UACPI_OBJECT_OPERATION_REGION_BIT;
		break;
	case ACPI_TYPE_POWER:
		*out = UACPI_OBJECT_POWER_RESOURCE_BIT;
		break;
	case ACPI_TYPE_PROCESSOR:
		*out = UACPI_OBJECT_PROCESSOR_BIT;
		break;
	case ACPI_TYPE_THERMAL:
		*out = UACPI_OBJECT_THERMAL_ZONE_BIT;
		break;
	case ACPI_TYPE_BUFFER_FIELD:
		*out = UACPI_OBJECT_BUFFER_FIELD_BIT;
		break;
	case ACPI_TYPE_DEBUG_OBJECT:
		*out = UACPI_OBJECT_DEBUG_BIT;
		break;
	default:
		shim_error(
			"unrecognized type %x given to acpi_type_to_uacpi_type_bits",
			in);
		return AE_NOT_IMPLEMENTED;
	}

	return AE_OK;
}
