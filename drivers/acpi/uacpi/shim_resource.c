#include <linux/acpi.h>

acpi_status acpi_get_current_resources(acpi_handle device_handle,
				       struct acpi_buffer *ret_buffer)
{
	BUG();
}

acpi_status acpi_get_possible_resources(acpi_handle device_handle,
					struct acpi_buffer *ret_buffer)
{
	BUG();
}

acpi_status acpi_set_current_resources(acpi_handle device_handle,
				       struct acpi_buffer *in_buffer)
{
	BUG();
}

acpi_status acpi_get_event_resources(acpi_handle device_handle,
				     struct acpi_buffer *ret_buffer)
{
	BUG();
}

acpi_status acpi_get_irq_routing_table(acpi_handle device_handle,
				       struct acpi_buffer *ret_buffer)
{
	BUG();
}

acpi_status acpi_get_vendor_resource(acpi_handle device_handle, char *name,
				     struct acpi_vendor_uuid *uuid,
				     struct acpi_buffer *ret_buffer)
{
	BUG();
}

acpi_status acpi_buffer_to_resource(u8 *aml_buffer, u16 aml_buffer_length,
				    struct acpi_resource **resource_ptr)
{
	BUG();
}

acpi_status acpi_resource_to_address64(struct acpi_resource *resource,
				       struct acpi_resource_address64 *out)
{
	BUG();
}

acpi_status acpi_walk_resource_buffer(struct acpi_buffer *buffer,
				      acpi_walk_resource_callback user_function,
				      void *context)
{
	BUG();
}

acpi_status acpi_walk_resources(acpi_handle device_handle, char *name,
				acpi_walk_resource_callback user_function,
				void *context)
{
	BUG();
}
