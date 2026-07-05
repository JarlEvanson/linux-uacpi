#include <linux/acpi.h>

acpi_status ACPI_INIT_FUNCTION
acpi_find_root_pointer(acpi_physical_address *table_address)
{
	BUG();
}

acpi_status ACPI_INIT_FUNCTION
acpi_initialize_tables(struct acpi_table_desc *initial_table_array,
		       u32 initial_table_count, u8 allow_resize)
{
	BUG();
}

acpi_status ACPI_INIT_FUNCTION acpi_reallocate_root_table(void)
{
	BUG();
}

acpi_status ACPI_INIT_FUNCTION
acpi_install_table(struct acpi_table_header *table)
{
	BUG();
}

acpi_status ACPI_INIT_FUNCTION
acpi_install_physical_table(acpi_physical_address address)
{
	BUG();
}

acpi_status acpi_load_table(struct acpi_table_header *table, u32 *table_idx)
{
	BUG();
}

acpi_status acpi_unload_parent_table(acpi_handle object)
{
	BUG();
}

acpi_status acpi_unload_table(u32 table_index)
{
	BUG();
}

acpi_status acpi_get_table(char *signature, u32 instance,
			   struct acpi_table_header **out_table)
{
	BUG();
}

void acpi_put_table(struct acpi_table_header *table)
{
	BUG();
}

acpi_status acpi_get_table_header(char *signature, u32 instance,
				  struct acpi_table_header *out_table_header)
{
	BUG();
}

acpi_status acpi_get_table_by_index(u32 table_index,
				    struct acpi_table_header **out_table)
{
	BUG();
}

acpi_status acpi_install_table_handler(acpi_table_handler handler,
				       void *context)
{
	BUG();
}

acpi_status acpi_remove_table_handler(acpi_table_handler handler)
{
	BUG();
}
