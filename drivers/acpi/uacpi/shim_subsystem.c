#include <linux/acpi.h>

#include <uacpi/uacpi.h>

#include "shim_shared.h"

acpi_status ACPI_INIT_FUNCTION acpi_initialize_subsystem(void)
{
	acpi_status acpi_st;
	uacpi_status st;
	uacpi_u64 flags;

	acpi_current_gpe_count = 0;

	acpi_st = acpi_os_initialize();
	if (acpi_st != AE_OK)
		return acpi_st;

	if (!acpi_gbl_create_osi_method)
		flags |= UACPI_FLAG_NO_OSI;
	if (acpi_gbl_enable_table_validation)
		flags |= UACPI_FLAG_PROACTIVE_TBL_CSUM;
	if (acpi_gbl_do_not_use_xsdt)
		flags |= UACPI_FLAG_BAD_XSDT;

	flags |= UACPI_FLAG_NO_ACPI_MODE;
	st = uacpi_initialize(flags);
	if (st != UACPI_STATUS_OK)
		return uacpi_to_acpi_status(st);

	return AE_OK;
}

acpi_status ACPI_INIT_FUNCTION acpi_enable_subsystem(u32 flags)
{
	uacpi_status st;

	// TODO: handle additional flags.
	if (!(flags & ACPI_NO_ACPI_ENABLE)) {
		st = uacpi_enter_acpi_mode();
		if (st != UACPI_STATUS_OK)
			return uacpi_to_acpi_status(st);
	}

	return AE_OK;
}

acpi_status ACPI_INIT_FUNCTION acpi_load_tables(void)
{
	uacpi_status st;

	st = uacpi_namespace_load();
	return uacpi_to_acpi_status(st);
}

acpi_status ACPI_INIT_FUNCTION acpi_initialize_objects(u32 flags)
{
	uacpi_status st;

	// TODO: handle additional flags.
	if (flags != ACPI_FULL_INITIALIZATION) {
		shim_error("acpi_initialize_objects(%x) is not supported",
			   flags);
		return AE_SUPPORT;
	}

	st = uacpi_namespace_initialize();
	return uacpi_to_acpi_status(st);
}

acpi_status acpi_subsystem_status(void)
{
	uacpi_init_level level;

	level = uacpi_get_current_init_level();
	switch (level) {
	case UACPI_INIT_LEVEL_NAMESPACE_INITIALIZED:
		return AE_OK;
	default:
		return AE_ERROR;
	}
}

acpi_status ACPI_INIT_FUNCTION acpi_terminate(void)
{
	uacpi_state_reset();
	return AE_OK;
}
