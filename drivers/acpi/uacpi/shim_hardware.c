#include <linux/acpi.h>

#include <uacpi/io.h>
#include <uacpi/registers.h>
#include <uacpi/sleep.h>
#include <uacpi/uacpi.h>

#include "shim_shared.h"

acpi_status acpi_enable(void)
{
	uacpi_status st;

	st = uacpi_enter_acpi_mode();
	return uacpi_to_acpi_status(st);
}

acpi_status acpi_disable(void)
{
	uacpi_status st;

	st = uacpi_leave_acpi_mode();
	return uacpi_to_acpi_status(st);
}

acpi_status acpi_reset(void)
{
	uacpi_status st;

	st = uacpi_reboot();
	return uacpi_to_acpi_status(st);
}

static acpi_status map_bitreg_to_uacpi(u32 register_id,
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
		shim_warn("map_bitreg_to_uacpi does not support register id %u",
			  register_id);
		return AE_NOT_IMPLEMENTED;
	}

	return AE_OK;
}

acpi_status acpi_read_bit_register(u32 register_id, u32 *return_value)
{
	uacpi_register_field field;
	uacpi_u64 val64;
	acpi_status acpi_st;
	uacpi_status st;

	if (!return_value)
		return AE_BAD_PARAMETER;

	acpi_st = map_bitreg_to_uacpi(register_id, &field);
	if (ACPI_FAILURE(acpi_st))
		return acpi_st;

	st = uacpi_read_register_field(field, &val64);
	if (st != UACPI_STATUS_OK)
		return uacpi_to_acpi_status(st);

	*return_value = (u32)val64;
	return AE_OK;
}

acpi_status acpi_write_bit_register(u32 register_id, u32 value)
{
	uacpi_register_field field;
	acpi_status acpi_st;
	uacpi_status st;

	acpi_st = map_bitreg_to_uacpi(register_id, &field);
	if (ACPI_FAILURE(acpi_st))
		return acpi_st;

	st = uacpi_write_register_field(field, (uacpi_u64)value);
	if (st != UACPI_STATUS_OK)
		return uacpi_to_acpi_status(st);

	return AE_OK;
}

/* Copied from uacpi/acpi.h to avoid name collisions */
UACPI_PACKED(struct acpi_gas {
	uacpi_u8 address_space_id;
	uacpi_u8 register_bit_width;
	uacpi_u8 register_bit_offset;
	uacpi_u8 access_size;
	uacpi_u64 address;
})
UACPI_EXPECT_SIZEOF(struct acpi_gas, 12);

acpi_status acpi_read(u64 *return_value, struct acpi_generic_address *reg)
{
	struct acpi_gas gas;
	uacpi_status st;

	gas.address_space_id = reg->space_id;
	gas.register_bit_width = reg->bit_width;
	gas.register_bit_offset = reg->bit_offset;
	gas.access_size = reg->access_width;
	gas.address = reg->address;

	st = uacpi_gas_read(&gas, return_value);
	return uacpi_to_acpi_status(st);
}

acpi_status acpi_write(u64 value, struct acpi_generic_address *reg)
{
	struct acpi_gas gas;
	uacpi_status st;

	gas.address_space_id = reg->space_id;
	gas.register_bit_width = reg->bit_width;
	gas.register_bit_offset = reg->bit_offset;
	gas.access_size = reg->access_width;
	gas.address = reg->address;

	st = uacpi_gas_write(&gas, value);
	return uacpi_to_acpi_status(st);
}

acpi_status acpi_acquire_global_lock(u16 timeout, u32 *handle)
{
	uacpi_status st;

	st = uacpi_acquire_global_lock(timeout, handle);
	return uacpi_to_acpi_status(st);
}

acpi_status acpi_release_global_lock(u32 handle)
{
	uacpi_status st;

	st = uacpi_release_global_lock(handle);
	return uacpi_to_acpi_status(st);
}

acpi_status acpi_get_timer_resolution(u32 *resolution)
{
	if (!resolution)
		return AE_BAD_PARAMETER;

	*resolution = (acpi_gbl_FADT.flags & ACPI_FADT_32BIT_TIMER) ? 32 : 24;
	return AE_OK;
}

acpi_status acpi_get_timer_duration(u32 start_ticks, u32 end_ticks,
				    u32 *time_elapsed)
{
	u32 resolution, delta;
	acpi_status acpi_st;

	if (!time_elapsed)
		return AE_BAD_PARAMETER;

	acpi_st = acpi_get_timer_resolution(&resolution);
	if (ACPI_FAILURE(acpi_st))
		return AE_BAD_PARAMETER;

	if (start_ticks < end_ticks)
		delta = end_ticks - start_ticks;
	else if (start_ticks > end_ticks)
		delta = (((resolution == 32) ? 0xFFFFFFFF : 0x00FFFFFF) -
			 start_ticks) +
			end_ticks + 1;
	else
		delta = 0;

	*time_elapsed = (u32)div_u64((u64)delta * 286331153ULL, 1000000000ULL);
	return AE_OK;
}

acpi_status acpi_get_timer(u32 *ticks)
{
	uacpi_u64 val;
	uacpi_status st;

	if (!ticks)
		return AE_BAD_PARAMETER;

	st = uacpi_read_register(UACPI_REGISTER_PM_TMR, &val);
	if (st != UACPI_STATUS_OK)
		return uacpi_to_acpi_status(st);

	*ticks = val;
	return AE_OK;
}
