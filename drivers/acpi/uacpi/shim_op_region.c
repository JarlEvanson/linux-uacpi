#include <linux/acpi.h>

#include <uacpi/kernel_api.h>
#include <uacpi/opregion.h>

#include "shim_shared.h"

static acpi_status acpi_to_uacpi_address_space(acpi_adr_space_type in,
					       uacpi_address_space *out)
{
	switch (in) {
	case ACPI_ADR_SPACE_SYSTEM_MEMORY:
		*out = UACPI_ADDRESS_SPACE_SYSTEM_MEMORY;
		break;
	case ACPI_ADR_SPACE_SYSTEM_IO:
		*out = UACPI_ADDRESS_SPACE_SYSTEM_IO;
		break;
	case ACPI_ADR_SPACE_PCI_CONFIG:
		*out = UACPI_ADDRESS_SPACE_PCI_CONFIG;
		break;
	case ACPI_ADR_SPACE_EC:
		*out = UACPI_ADDRESS_SPACE_EMBEDDED_CONTROLLER;
		break;
	case ACPI_ADR_SPACE_SMBUS:
		*out = UACPI_ADDRESS_SPACE_SMBUS;
		break;
	case ACPI_ADR_SPACE_CMOS:
		*out = UACPI_ADDRESS_SPACE_SYSTEM_CMOS;
		break;
	case ACPI_ADR_SPACE_PCI_BAR_TARGET:
		*out = UACPI_ADDRESS_SPACE_PCI_BAR_TARGET;
		break;
	case ACPI_ADR_SPACE_IPMI:
		*out = UACPI_ADDRESS_SPACE_IPMI;
		break;
	case ACPI_ADR_SPACE_GPIO:
		*out = UACPI_ADDRESS_SPACE_GENERAL_PURPOSE_IO;
		break;
	case ACPI_ADR_SPACE_GSBUS:
		*out = UACPI_ADDRESS_SPACE_GENERIC_SERIAL_BUS;
		break;
	case ACPI_ADR_SPACE_PLATFORM_COMM:
		*out = UACPI_ADDRESS_SPACE_PCC;
		break;
	case ACPI_ADR_SPACE_PLATFORM_RT:
		*out = UACPI_ADDRESS_SPACE_PRM;
		break;
	case ACPI_ADR_SPACE_FIXED_HARDWARE:
		*out = UACPI_ADDRESS_SPACE_FFIXEDHW;
		break;
	default:
		acpi_os_printf(
			"ACPI: WARN: acpi_to_uacpi_address_space does not support address space id %u\n",
			in);
		return AE_SUPPORT;
	}

	return AE_OK;
}

struct shim_region_handler_context {
	acpi_adr_space_setup setup;
	acpi_adr_space_handler handler;

	void *context;
};

static uacpi_status shim_region_handler(uacpi_region_op op,
					uacpi_handle op_context)
{
	acpi_os_printf("ACPI: shim_region_handler(%u, %px)\n", op, op_context);

	BUG();
}

acpi_status acpi_install_address_space_handler(acpi_handle device,
					       acpi_adr_space_type space_id,
					       acpi_adr_space_handler handler,
					       acpi_adr_space_setup setup,
					       void *context)
{
	{
		const uacpi_char *absolute_path =
			uacpi_namespace_node_generate_absolute_path(
				uacpi_node_from_handle(device));

		acpi_os_printf(
			"ACPI: acpi_install_address_space_handler(%s, %u %px, %px, %px)\n",
			absolute_path, space_id, handler, setup, context);

		uacpi_free_absolute_path(absolute_path);
	}

	uacpi_address_space address_space;
	acpi_object_type device_type;
	struct shim_region_handler_context *shim_context;
	acpi_status acpi_st;
	uacpi_status uacpi_st;

	if (device != ACPI_ROOT_OBJECT) {
		acpi_st = acpi_get_type(device, &device_type);
		if (acpi_st != AE_OK)
			return acpi_st;

		switch (device_type) {
		case ACPI_TYPE_DEVICE:
		case ACPI_TYPE_PROCESSOR:
		case ACPI_TYPE_THERMAL:
			break;
		default:
			return AE_BAD_PARAMETER;
		}
	}

	acpi_st = acpi_to_uacpi_address_space(space_id, &address_space);
	if (acpi_st != AE_OK)
		return acpi_st;

	if (handler == ACPI_DEFAULT_HANDLER) {
		acpi_os_printf(
			"ACPI: WARN: uACPI shim does not support default address space handlers\n");
		return AE_SUPPORT;
	}

	shim_context = uacpi_kernel_alloc(sizeof(*shim_context));
	if (!shim_context)
		return AE_NO_MEMORY;

	shim_context->setup = setup;
	shim_context->handler = handler;
	shim_context->context = context;

	uacpi_st = uacpi_install_address_space_handler(
		uacpi_node_from_handle(device), address_space,
		shim_region_handler, shim_context);
	if (uacpi_st != UACPI_STATUS_OK) {
		uacpi_kernel_free(shim_context);
		return uacpi_convert_status(uacpi_st);
	}

	return AE_OK;
}

acpi_status acpi_install_address_space_handler_no_reg(
	acpi_handle device, acpi_adr_space_type space_id,
	acpi_adr_space_handler handler, acpi_adr_space_setup setup,
	void *context)
{
	BUG();
}

acpi_status acpi_remove_address_space_handler(acpi_handle device,
					      acpi_adr_space_type space_id,
					      acpi_adr_space_handler handler)
{
	BUG();
}

acpi_status acpi_execute_reg_methods(acpi_handle device, u32 max_depth,
				     acpi_adr_space_type space_id)
{
	BUG();
}
