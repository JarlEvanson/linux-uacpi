#include <linux/acpi.h>

acpi_status acpi_evaluate_object(acpi_handle handle, acpi_string pathname,
				 struct acpi_object_list *external_params,
				 struct acpi_buffer *return_buffer)
{
	BUG();
}

acpi_status acpi_evaluate_object_typed(acpi_handle handle, acpi_string pathname,
				       struct acpi_object_list *external_params,
				       struct acpi_buffer *return_buffer,
				       acpi_object_type return_type)
{
	BUG();
}

acpi_status acpi_get_type(acpi_handle handle, acpi_object_type *ret_type)
{
	BUG();
}

acpi_status acpi_get_object_info(acpi_handle handle,
				 struct acpi_device_info **return_buffer)
{
	BUG();
}

acpi_status acpi_acquire_mutex(acpi_handle handle, acpi_string pathname,
			       u16 timeout)
{
	BUG();
}

acpi_status acpi_release_mutex(acpi_handle handle, acpi_string pathname)
{
	BUG();
}
