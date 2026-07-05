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
