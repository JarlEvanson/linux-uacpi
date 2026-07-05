#include <linux/acpi.h>

acpi_status acpi_get_handle(acpi_handle parent, const char *pathname,
			    acpi_handle *ret_handle)
{
	BUG();
}

acpi_status acpi_get_name(acpi_handle handle, u32 name_type,
			  struct acpi_buffer *buffer)
{
	BUG();
}

acpi_status acpi_get_parent(acpi_handle handle, acpi_handle *ret_handle)
{
	BUG();
}

acpi_status acpi_get_next_object(acpi_object_type type, acpi_handle parent,
				 acpi_handle child, acpi_handle *ret_handle)
{
	BUG();
}

acpi_status acpi_install_method(u8 *buffer)
{
	BUG();
}

acpi_status acpi_walk_namespace(acpi_object_type type, acpi_handle start_object,
				u32 max_depth,
				acpi_walk_callback descending_callback,
				acpi_walk_callback ascending_callback,
				void *context, void **return_value)
{
	BUG();
}

acpi_status acpi_get_devices(const char *HID, acpi_walk_callback user_function,
			     void *context, void **return_value)
{
	BUG();
}

acpi_status acpi_attach_data(acpi_handle obj_handle,
			     acpi_object_handler handler, void *data)
{
	BUG();
}

acpi_status acpi_detach_data(acpi_handle obj_handle,
			     acpi_object_handler handler)
{
	BUG();
}

acpi_status acpi_get_data_full(acpi_handle obj_handle,
			       acpi_object_handler handler, void **data,
			       void (*callback)(void *))
{
	BUG();
}

acpi_status acpi_get_data(acpi_handle obj_handle, acpi_object_handler handler,
			  void **data)
{
	BUG();
}
