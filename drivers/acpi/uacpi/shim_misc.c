#include <linux/acpi.h>

u32 acpi_check_address_range(acpi_adr_space_type space_id,
			     acpi_physical_address address, acpi_size length,
			     u8 warn)
{
	BUG();
}

acpi_status acpi_debug_trace(const char *name, u32 debug_level, u32 debug_layer,
			     u32 flags)
{
	BUG();
}

acpi_status acpi_decode_pld_buffer(u8 *in_buffer, acpi_size length,
				   struct acpi_pld_info **return_buffer)
{
	BUG();
}

const char *acpi_format_exception(acpi_status status)
{
	BUG();
}

acpi_status acpi_get_statistics(struct acpi_statistics *stats)
{
	BUG();
}

acpi_status acpi_get_system_info(struct acpi_buffer *out_buffer)
{
	BUG();
}

acpi_status acpi_purge_cached_objects(void)
{
	BUG();
}
