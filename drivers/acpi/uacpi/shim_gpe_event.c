#include <linux/acpi.h>

acpi_status acpi_enable_gpe(acpi_handle gpe_device, u32 gpe_number)
{
	BUG();
}

acpi_status acpi_enable_gpe_cond(acpi_handle gpe_device, u32 gpe_number,
				 u8 dispatch_type)
{
	BUG();
}

acpi_status acpi_disable_gpe(acpi_handle gpe_device, u32 gpe_number)
{
	BUG();
}

acpi_status acpi_clear_gpe(acpi_handle gpe_device, u32 gpe_number)
{
	BUG();
}

acpi_status acpi_set_gpe(acpi_handle gpe_device, u32 gpe_number, u8 action)
{
	BUG();
}

acpi_status acpi_mask_gpe(acpi_handle gpe_device, u32 gpe_number, u8 is_masked)
{
	BUG();
}

acpi_status acpi_finish_gpe(acpi_handle gpe_device, u32 gpe_number)
{
	BUG();
}

acpi_status acpi_setup_gpe_for_wake(acpi_handle wake_device,
				    acpi_handle gpe_device, u32 gpe_number)
{
	BUG();
}

acpi_status acpi_mark_gpe_for_wake(acpi_handle gpe_device, u32 gpe_number)
{
	BUG();
}

acpi_status acpi_set_gpe_wake_mask(acpi_handle gpe_device, u32 gpe_number,
				   u8 action)
{
	BUG();
}

acpi_status acpi_get_gpe_status(acpi_handle gpe_device, u32 gpe_number,
				acpi_event_status *event_status)
{
	BUG();
}

acpi_status acpi_get_gpe_device(u32 index, acpi_handle *gpe_device)
{
	BUG();
}

u32 acpi_any_gpe_status_set(u32 gpe_skip_number)
{
	BUG();
}

acpi_status acpi_update_all_gpes(void)
{
	BUG();
}

acpi_status acpi_disable_all_gpes(void)
{
	BUG();
}

acpi_status acpi_hw_disable_all_gpes(void)
{
	BUG();
}

acpi_status acpi_enable_all_runtime_gpes(void)
{
	BUG();
}

acpi_status acpi_enable_all_wakeup_gpes(void)
{
	BUG();
}

acpi_status
acpi_install_gpe_block(acpi_handle gpe_device,
		       struct acpi_generic_address *gpe_block_address,
		       u32 register_count, u32 interrupt_number)
{
	BUG();
}

acpi_status acpi_remove_gpe_block(acpi_handle gpe_device)
{
	BUG();
}

acpi_status acpi_install_gpe_handler(acpi_handle gpe_device, u32 gpe_number,
				     u32 type, acpi_gpe_handler address,
				     void *context)
{
	BUG();
}

acpi_status acpi_install_gpe_raw_handler(acpi_handle gpe_device, u32 gpe_number,
					 u32 type, acpi_gpe_handler address,
					 void *context)
{
	BUG();
}

acpi_status acpi_remove_gpe_handler(acpi_handle gpe_device, u32 gpe_number,
				    acpi_gpe_handler address)
{
	BUG();
}
