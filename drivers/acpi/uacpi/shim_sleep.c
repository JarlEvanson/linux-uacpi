#include <linux/acpi.h>

#include <uacpi/sleep.h>

#include "shim_shared.h"

static uacpi_status acpi_to_uacpi_sleep_state(u8 sleep_state,
					      uacpi_sleep_state *out)
{
	switch (sleep_state) {
	case ACPI_STATE_S0:
		*out = UACPI_SLEEP_STATE_S0;
		break;
	case ACPI_STATE_S1:
		*out = UACPI_SLEEP_STATE_S1;
		break;
	case ACPI_STATE_S2:
		*out = UACPI_SLEEP_STATE_S2;
		break;
	case ACPI_STATE_S3:
		*out = UACPI_SLEEP_STATE_S3;
		break;
	case ACPI_STATE_S4:
		*out = UACPI_SLEEP_STATE_S4;
		break;
	case ACPI_STATE_S5:
		*out = UACPI_SLEEP_STATE_S5;
		break;
	default:
		acpi_os_printf("uACPI does not recognize sleep state %x",
			       sleep_state);
		return UACPI_STATUS_UNIMPLEMENTED;
	}

	return UACPI_STATUS_OK;
}

acpi_status
acpi_set_firmware_waking_vector(acpi_physical_address physical_address,
				acpi_physical_address physical_address64)
{
	uacpi_status st;

	st = uacpi_set_waking_vector(physical_address, physical_address64);
	return uacpi_convert_status(st);
}

acpi_status acpi_enter_sleep_state_prep(u8 sleep_state)
{
	uacpi_sleep_state state;
	uacpi_status st;

	st = acpi_to_uacpi_sleep_state(sleep_state, &state);
	if (st != UACPI_STATUS_OK)
		return uacpi_convert_status(st);

	st = uacpi_prepare_for_sleep_state(state);
	return uacpi_convert_status(st);
}

acpi_status acpi_leave_sleep_state_prep(u8 sleep_state)
{
	uacpi_sleep_state state;
	uacpi_status st;

	st = acpi_to_uacpi_sleep_state(sleep_state, &state);
	if (st != UACPI_STATUS_OK)
		return uacpi_convert_status(st);

	st = uacpi_prepare_for_wake_from_sleep_state(state);
	return uacpi_convert_status(st);
}

acpi_status acpi_enter_sleep_state(u8 sleep_state)
{
	uacpi_sleep_state state;
	uacpi_status st;

	st = acpi_to_uacpi_sleep_state(sleep_state, &state);
	if (st != UACPI_STATUS_OK)
		return uacpi_convert_status(st);

	st = uacpi_enter_sleep_state(state);
	return uacpi_convert_status(st);
}

acpi_status acpi_enter_sleep_state_s4bios(void)
{
	BUG();
}

acpi_status acpi_leave_sleep_state(u8 sleep_state)
{
	uacpi_sleep_state state;
	uacpi_status st;

	st = acpi_to_uacpi_sleep_state(sleep_state, &state);
	if (st != UACPI_STATUS_OK)
		return uacpi_convert_status(st);

	st = uacpi_wake_from_sleep_state(state);
	return uacpi_convert_status(st);
}

acpi_status acpi_get_sleep_type_data(u8 sleep_state, u8 *sleep_type_a,
				     u8 *sleep_type_b)
{
	uacpi_sleep_state state;
	uacpi_object *sleep_package, *package_object;
	uacpi_object_array sleep_package_contents;
	uacpi_u64 val;
	uacpi_status st;
	char path[] = "_Sx";

	if (!sleep_type_a || !sleep_type_b)
		return AE_BAD_PARAMETER;

	st = acpi_to_uacpi_sleep_state(sleep_state, &state);
	if (st != UACPI_STATUS_OK)
		return uacpi_convert_status(st);

	path[2] = '0' + (char)state;
	st = uacpi_eval_simple_package(uacpi_namespace_root(), path,
				       &sleep_package);
	if (st != UACPI_STATUS_OK)
		return uacpi_convert_status(st);

	st = uacpi_object_get_package(sleep_package, &sleep_package_contents);
	if (st != UACPI_STATUS_OK) {
		uacpi_object_unref(sleep_package);
		return uacpi_convert_status(st);
	}

	switch (sleep_package_contents.count) {
	case 0:
		uacpi_object_unref(sleep_package);
		return AE_AML_PACKAGE_LIMIT;
	case 1:
		package_object = sleep_package_contents.objects[0];
		if (!uacpi_object_is(package_object, UACPI_OBJECT_INTEGER)) {
			uacpi_object_unref(sleep_package);
			return AE_AML_OPERAND_TYPE;
		}

		st = uacpi_object_get_integer(package_object, &val);
		if (st != UACPI_STATUS_OK) {
			uacpi_object_unref(sleep_package);
			return uacpi_convert_status(st);
		}

		*sleep_type_a = (u8)val;
		*sleep_type_b = (u8)(val >> 8);
		uacpi_object_unref(sleep_package);
		return AE_OK;
	default:
		package_object = sleep_package_contents.objects[0];

		st = uacpi_object_get_integer(package_object, &val);
		if (st != UACPI_STATUS_OK) {
			uacpi_object_unref(sleep_package);
			return uacpi_convert_status(st);
		}

		*sleep_type_a = (u8)val;

		package_object = sleep_package_contents.objects[1];

		st = uacpi_object_get_integer(package_object, &val);
		if (st != UACPI_STATUS_OK) {
			uacpi_object_unref(sleep_package);
			return uacpi_convert_status(st);
		}

		*sleep_type_b = (u8)val;
		uacpi_object_unref(sleep_package);
		return AE_OK;
	}
}
