#include <linux/acpi.h>

#include <uacpi/notify.h>

#include "shim_shared.h"

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

static uacpi_status shim_notify_handler(uacpi_handle context,
					uacpi_namespace_node *node,
					uacpi_u64 value)
{
	BUG();
}

acpi_status acpi_install_notify_handler(acpi_handle device, u32 handler_type,
					acpi_notify_handler handler,
					void *context)
{
	uacpi_namespace_node *node;
	uacpi_status st;

	acpi_os_printf(
		"ACPI: WARN: acpi_install_notify_handler is not fully implemented\n");
	{
		const uacpi_char *path =
			uacpi_namespace_node_generate_absolute_path(
				uacpi_node_from_handle(device));
		acpi_os_printf(
			"acpi_install_notify_handler(%s, %u, %px, %px)\n", path,
			handler_type, handler, context);
	}

	node = uacpi_node_from_handle(device);
	st = uacpi_install_notify_handler(node, shim_notify_handler, context);
	return uacpi_convert_status(st);
}

acpi_status acpi_remove_notify_handler(acpi_handle device, u32 handler_type,
				       acpi_notify_handler handler)
{
	uacpi_namespace_node *node;
	uacpi_status st;

	node = uacpi_node_from_handle(device);
	st = uacpi_uninstall_notify_handler(node, shim_notify_handler);
	return uacpi_convert_status(st);
}

acpi_status acpi_install_exception_handler(acpi_exception_handler handler)
{
	return AE_SUPPORT;
}
