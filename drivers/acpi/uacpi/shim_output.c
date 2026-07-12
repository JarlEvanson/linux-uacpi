#include <linux/acpi.h>

#include "shim_shared.h"

void ACPI_INTERNAL_VAR_XFACE acpi_info(const char *format, ...)
{
	va_list args;
	char buffer[512];
	int printed;

	va_start(args, format);
	printed = vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	if (printed <= 0)
		return;

	shim_info("%s", buffer);
}

void ACPI_INTERNAL_VAR_XFACE acpi_warning(const char *module_name,
					  u32 line_number, const char *format,
					  ...)
{
	va_list args;
	char buffer[512];
	int printed;

	va_start(args, format);
	printed = vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	if (printed <= 0)
		return;

	shim_warn("%s:%u: %s", module_name, line_number, buffer);
}

void ACPI_INTERNAL_VAR_XFACE acpi_error(const char *module_name,
					u32 line_number, const char *format,
					...)
{
	va_list args;
	char buffer[512];
	int printed;

	va_start(args, format);
	printed = vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	if (printed <= 0)
		return;

	shim_error("%s:%u: %s", module_name, line_number, buffer);
}

void ACPI_INTERNAL_VAR_XFACE acpi_exception(const char *module_name,
					    u32 line_number, acpi_status status,
					    const char *format, ...)
{
	va_list args;
	char buffer[512];
	int printed;
	const char *formatted_status;

	va_start(args, format);
	printed = vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	if (printed <= 0)
		return;

	formatted_status = acpi_format_exception(status);
	shim_error("%s:%u: %s (%s)", module_name, line_number, buffer,
		   formatted_status);
}

void ACPI_INTERNAL_VAR_XFACE acpi_bios_warning(const char *module_name,
					       u32 line_number,
					       const char *format, ...)
{
	va_list args;
	char buffer[512];
	int printed;

	va_start(args, format);
	printed = vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	if (printed <= 0)
		return;

	shim_warn("%s:%u: ACPI Firmware Warning: %s", module_name, line_number,
		  buffer);
}

void ACPI_INTERNAL_VAR_XFACE acpi_bios_error(const char *module_name,
					     u32 line_number,
					     const char *format, ...)
{
	va_list args;
	char buffer[512];
	int printed;

	va_start(args, format);
	printed = vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	if (printed <= 0)
		return;

	shim_error("%s:%u: ACPI Firmware Error: %s", module_name, line_number,
		   buffer);
}

void ACPI_INTERNAL_VAR_XFACE acpi_bios_exception(const char *module_name,
						 u32 line_number,
						 acpi_status status,
						 const char *format, ...)
{
	va_list args;
	char buffer[512];
	int printed;
	const char *formatted_status;

	va_start(args, format);
	printed = vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	if (printed <= 0)
		return;

	formatted_status = acpi_format_exception(status);
	shim_error("%s:%u: ACPI Firmware Error: %s (%s)", module_name,
		   line_number, buffer, formatted_status);
}
