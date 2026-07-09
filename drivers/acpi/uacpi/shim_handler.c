#include <linux/acpi.h>

acpi_status acpi_install_sci_handler(acpi_sci_handler address, void *context)
{
	return AE_SUPPORT;
}

acpi_status acpi_remove_sci_handler(acpi_sci_handler address)
{
	return AE_SUPPORT;
}

acpi_status acpi_install_global_event_handler(acpi_gbl_event_handler handler,
					      void *context)
{
	return AE_SUPPORT;
}

acpi_status acpi_install_notify_handler(acpi_handle device, u32 handler_type,
					acpi_notify_handler handler,
					void *context)
{
	return AE_SUPPORT;
}

acpi_status acpi_remove_notify_handler(acpi_handle device, u32 handler_type,
				       acpi_notify_handler handler)
{
	return AE_SUPPORT;
}

acpi_status acpi_install_exception_handler(acpi_exception_handler handler)
{
	return AE_SUPPORT;
}
