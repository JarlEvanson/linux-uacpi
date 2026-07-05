#include <linux/acpi.h>

acpi_status acpi_enable_event(u32 event, u32 flags)
{
	BUG();
}

acpi_status acpi_disable_event(u32 event, u32 flags)
{
	BUG();
}

acpi_status acpi_clear_event(u32 event)
{
	BUG();
}

acpi_status acpi_get_event_status(u32 event, acpi_event_status *event_status)
{
	BUG();
}

u32 acpi_any_fixed_event_status_set(void)
{
	BUG();
}

acpi_status acpi_install_fixed_event_handler(u32 event,
					     acpi_event_handler handler,
					     void *context)
{
	BUG();
}

acpi_status acpi_remove_fixed_event_handler(u32 event,
					    acpi_event_handler handler)
{
	BUG();
}
