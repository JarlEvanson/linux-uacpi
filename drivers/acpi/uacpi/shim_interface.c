#include <linux/acpi.h>

#include <uacpi/osi.h>

#include "shim_shared.h"

static uacpi_host_interface shim_host_interface(const char *name)
{
	if (!strcmp(name, "Module Device"))
		return UACPI_HOST_INTERFACE_MODULE_DEVICE;
	if (!strcmp(name, "Processor Device"))
		return UACPI_HOST_INTERFACE_PROCESSOR_DEVICE;
	if (!strcmp(name, "3.0 Thermal Model"))
		return UACPI_HOST_INTERFACE_3_0_THERMAL_MODEL;
	if (!strcmp(name, "3.0 _SCP Extensions"))
		return UACPI_HOST_INTERFACE_3_0_SCP_EXTENSIONS;
	if (!strcmp(name, "Processor Aggregator Device"))
		return UACPI_HOST_INTERFACE_PROCESSOR_AGGREGATOR_DEVICE;
	return 0;
}

acpi_status acpi_install_interface(acpi_string interface_name)
{
	uacpi_status st;

	uacpi_host_interface hi = shim_host_interface(interface_name);

	if (hi)
		st = uacpi_enable_host_interface(hi);
	else
		st = uacpi_install_interface(interface_name,
					     UACPI_INTERFACE_KIND_VENDOR);
	return uacpi_to_acpi_status(st);
}

acpi_status acpi_remove_interface(acpi_string interface_name)
{
	uacpi_status st;

	uacpi_host_interface hi = shim_host_interface(interface_name);

	if (hi)
		st = uacpi_disable_host_interface(hi);
	else
		st = uacpi_uninstall_interface(interface_name);
	return uacpi_to_acpi_status(st);
}

acpi_status acpi_update_interfaces(u8 action)
{
	uacpi_status st;

	switch (action) {
	case ACPI_DISABLE_ALL_VENDOR_STRINGS:
		st = uacpi_bulk_configure_interfaces(
			UACPI_INTERFACE_ACTION_DISABLE,
			UACPI_INTERFACE_KIND_VENDOR);
		break;
	case ACPI_DISABLE_ALL_FEATURE_STRINGS:
		st = uacpi_bulk_configure_interfaces(
			UACPI_INTERFACE_ACTION_DISABLE,
			UACPI_INTERFACE_KIND_FEATURE);
		break;
	case ACPI_DISABLE_ALL_STRINGS:
		st = uacpi_bulk_configure_interfaces(
			UACPI_INTERFACE_ACTION_DISABLE,
			UACPI_INTERFACE_KIND_ALL);
		break;
	case ACPI_ENABLE_ALL_VENDOR_STRINGS:
		st = uacpi_bulk_configure_interfaces(
			UACPI_INTERFACE_ACTION_ENABLE,
			UACPI_INTERFACE_KIND_VENDOR);
		break;
	case ACPI_ENABLE_ALL_FEATURE_STRINGS:
		st = uacpi_bulk_configure_interfaces(
			UACPI_INTERFACE_ACTION_ENABLE,
			UACPI_INTERFACE_KIND_FEATURE);
		break;
	case ACPI_ENABLE_ALL_STRINGS:
		st = uacpi_bulk_configure_interfaces(
			UACPI_INTERFACE_ACTION_ENABLE,
			UACPI_INTERFACE_KIND_ALL);
		break;
	default:
		return AE_BAD_PARAMETER;
	}

	return uacpi_to_acpi_status(st);
}

static DEFINE_SPINLOCK(shim_interface_handler_lock);
static acpi_interface_handler shim_interface_handler;

static uacpi_bool acpi_interface_handler_impl(const uacpi_char *name,
					      uacpi_bool supported)
{
	acpi_interface_handler handler;

	spin_lock(&shim_interface_handler_lock);
	handler = shim_interface_handler;
	spin_unlock(&shim_interface_handler_lock);

	if (!handler)
		return supported;

	return (uacpi_bool)handler((acpi_string)name, (u32)supported);
}

acpi_status acpi_install_interface_handler(acpi_interface_handler handler)
{
	uacpi_status st;

	spin_lock(&shim_interface_handler_lock);

	shim_interface_handler = handler;
	if (handler) {
		st = uacpi_set_interface_query_handler(
			acpi_interface_handler_impl);
	} else {
		st = uacpi_set_interface_query_handler(UACPI_NULL);
	}

	spin_unlock(&shim_interface_handler_lock);
	return uacpi_to_acpi_status(st);
}
