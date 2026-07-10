#include <linux/acpi.h>

#include <uacpi/registers.h>
#include <uacpi/sleep.h>
#include <uacpi/uacpi.h>

#include "shim_shared.h"

acpi_status acpi_enable(void)
{
	uacpi_status st;

	st = uacpi_enter_acpi_mode();
	return uacpi_convert_status(st);
}

acpi_status acpi_disable(void)
{
	uacpi_status st;

	st = uacpi_leave_acpi_mode();
	return uacpi_convert_status(st);
}

acpi_status acpi_reset(void)
{
	uacpi_status st;

	st = uacpi_reboot();
	return uacpi_convert_status(st);
}

static uacpi_status map_bitreg_to_uacpi(u32 register_id,
					uacpi_register_field *out_field)
{
	switch (register_id) {
	case ACPI_BITREG_TIMER_STATUS:
		*out_field = UACPI_REGISTER_FIELD_TMR_STS;
		break;
	case ACPI_BITREG_BUS_MASTER_STATUS:
		*out_field = UACPI_REGISTER_FIELD_BM_STS;
		break;
	case ACPI_BITREG_GLOBAL_LOCK_STATUS:
		*out_field = UACPI_REGISTER_FIELD_GBL_STS;
		break;
	case ACPI_BITREG_POWER_BUTTON_STATUS:
		*out_field = UACPI_REGISTER_FIELD_PWRBTN_STS;
		break;
	case ACPI_BITREG_SLEEP_BUTTON_STATUS:
		*out_field = UACPI_REGISTER_FIELD_SLPBTN_STS;
		break;
	case ACPI_BITREG_RT_CLOCK_STATUS:
		*out_field = UACPI_REGISTER_FIELD_RTC_STS;
		break;
	case ACPI_BITREG_PCIEXP_WAKE_STATUS:
		*out_field = UACPI_REGISTER_FIELD_PCIEX_WAKE_STS;
		break;
	case ACPI_BITREG_WAKE_STATUS:
		*out_field = UACPI_REGISTER_FIELD_WAK_STS;
		break;
	case ACPI_BITREG_TIMER_ENABLE:
		*out_field = UACPI_REGISTER_FIELD_TMR_EN;
		break;
	case ACPI_BITREG_GLOBAL_LOCK_ENABLE:
		*out_field = UACPI_REGISTER_FIELD_GBL_EN;
		break;
	case ACPI_BITREG_POWER_BUTTON_ENABLE:
		*out_field = UACPI_REGISTER_FIELD_PWRBTN_EN;
		break;
	case ACPI_BITREG_SLEEP_BUTTON_ENABLE:
		*out_field = UACPI_REGISTER_FIELD_SLPBTN_EN;
		break;
	case ACPI_BITREG_RT_CLOCK_ENABLE:
		*out_field = UACPI_REGISTER_FIELD_RTC_EN;
		break;
	case ACPI_BITREG_PCIEXP_WAKE_DISABLE:
		*out_field = UACPI_REGISTER_FIELD_PCIEXP_WAKE_DIS;
		break;
	case ACPI_BITREG_SCI_ENABLE:
		*out_field = UACPI_REGISTER_FIELD_SCI_EN;
		break;
	case ACPI_BITREG_BUS_MASTER_RLD:
		*out_field = UACPI_REGISTER_FIELD_BM_RLD;
		break;
	case ACPI_BITREG_GLOBAL_LOCK_RELEASE:
		*out_field = UACPI_REGISTER_FIELD_GBL_RLS;
		break;
	case ACPI_BITREG_SLEEP_TYPE:
		*out_field = UACPI_REGISTER_FIELD_SLP_TYP;
		break;
	case ACPI_BITREG_SLEEP_ENABLE:
		*out_field = UACPI_REGISTER_FIELD_SLP_EN;
		break;
	case ACPI_BITREG_ARB_DISABLE:
		*out_field = UACPI_REGISTER_FIELD_ARB_DIS;
		break;
	default:
		return UACPI_STATUS_INVALID_ARGUMENT;
	}
	return UACPI_STATUS_OK;
}

acpi_status acpi_read_bit_register(u32 register_id, u32 *return_value)
{
	uacpi_register_field field;
	uacpi_u64 val64;
	uacpi_status st;

	if (!return_value) {
		return AE_BAD_PARAMETER;
	}

	st = map_bitreg_to_uacpi(register_id, &field);
	if (st != UACPI_STATUS_OK) {
		return uacpi_convert_status(st);
	}

	st = uacpi_read_register_field(field, &val64);
	if (st != UACPI_STATUS_OK) {
		return uacpi_convert_status(st);
	}

	*return_value = (u32)val64;
	return AE_OK;
}

acpi_status acpi_write_bit_register(u32 register_id, u32 value)
{
	uacpi_register_field field;
	uacpi_status st;

	st = map_bitreg_to_uacpi(register_id, &field);
	if (st != UACPI_STATUS_OK) {
		return uacpi_convert_status(st);
	}

	st = uacpi_write_register_field(field, (uacpi_u64)value);
	if (st != UACPI_STATUS_OK) {
		return uacpi_convert_status(st);
	}

	return AE_OK;
}

acpi_status acpi_read(u64 *return_value, struct acpi_generic_address *reg)
{
	BUG();
}

acpi_status acpi_write(u64 value, struct acpi_generic_address *reg)
{
	BUG();
}

acpi_status acpi_acquire_global_lock(u16 timeout, u32 *handle)
{
	BUG();
}

acpi_status acpi_release_global_lock(u32 handle)
{
	BUG();
}

acpi_status acpi_get_timer_resolution(u32 *resolution)
{
	BUG();
}

acpi_status acpi_get_timer(u32 *ticks)
{
	BUG();
}

acpi_status acpi_get_timer_duration(u32 start_ticks, u32 end_ticks,
				    u32 *time_elapsed)
{
	BUG();
}
