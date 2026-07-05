#include <linux/acpi.h>

acpi_status acpi_install_address_space_handler(acpi_handle device,
					       acpi_adr_space_type space_id,
					       acpi_adr_space_handler handler,
					       acpi_adr_space_setup setup,
					       void *context)
{
	BUG();
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
