#include <linux/acpi.h>

void ACPI_INTERNAL_VAR_XFACE acpi_info(const char *format, ...)
{
	BUG();
}

void ACPI_INTERNAL_VAR_XFACE acpi_warning(const char *module_name,
					  u32 line_number, const char *format,
					  ...)
{
	BUG();
}

void ACPI_INTERNAL_VAR_XFACE acpi_error(const char *module_name,
					u32 line_number, const char *format,
					...)
{
	BUG();
}

void ACPI_INTERNAL_VAR_XFACE acpi_exception(const char *module_name,
					    u32 line_number, acpi_status status,
					    const char *format, ...)
{
	BUG();
}

void ACPI_INTERNAL_VAR_XFACE acpi_bios_warning(const char *module_name,
					       u32 line_number,
					       const char *format, ...)
{
	BUG();
}

void ACPI_INTERNAL_VAR_XFACE acpi_bios_error(const char *module_name,
					     u32 line_number,
					     const char *format, ...)
{
	BUG();
}

void ACPI_INTERNAL_VAR_XFACE acpi_bios_exception(const char *module_name,
						 u32 line_number,
						 acpi_status status,
						 const char *format, ...)
{
	BUG();
}

void ACPI_INTERNAL_VAR_XFACE acpi_debug_print(
	u32 requested_debug_level, u32 line_number, const char *function_name,
	const char *module_name, u32 component_id, const char *format, ...)
{
	BUG();
}

void ACPI_INTERNAL_VAR_XFACE acpi_debug_print_raw(
	u32 requested_debug_level, u32 line_number, const char *function_name,
	const char *module_name, u32 component_id, const char *format, ...)
{
	BUG();
}
