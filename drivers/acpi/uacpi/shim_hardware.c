#include <linux/acpi.h>

acpi_status acpi_enable(void)
{
	BUG();
}

acpi_status acpi_disable(void)
{
	BUG();
}

acpi_status acpi_reset(void)
{
	BUG();
}

acpi_status acpi_read_bit_register(u32 register_id, u32 *return_value)
{
	BUG();
}

acpi_status acpi_write_bit_register(u32 register_id, u32 value)
{
	BUG();
}

acpi_status acpi_read(u64 *return_value, struct acpi_generic_address *reg)
{
	BUG();
}

acpi_status acpi_write(u64 value, struct acpi_generic_address *reg)
{
	BUG();
}

acpi_status acpi_acquire_global_lock(u16 timeout, u32 *handle)
{
	BUG();
}

acpi_status acpi_release_global_lock(u32 handle)
{
	BUG();
}

acpi_status acpi_get_timer_resolution(u32 *resolution)
{
	BUG();
}

acpi_status acpi_get_timer(u32 *ticks)
{
	BUG();
}

acpi_status acpi_get_timer_duration(u32 start_ticks, u32 end_ticks,
				    u32 *time_elapsed)
{
	BUG();
}
