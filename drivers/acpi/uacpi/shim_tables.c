#include <linux/acpi.h>

#include <uacpi/uacpi.h>
#include <uacpi/tables.h>

#include "shim_shared.h"

static DEFINE_MUTEX(shim_table_array_lock);
static struct acpi_table_desc *shim_table_array = NULL;
static u32 shim_table_array_length = 0;

static u8 shim_early_table_buffer[16384] __initdata __aligned(8);

acpi_status ACPI_INIT_FUNCTION
acpi_find_root_pointer(acpi_physical_address *table_address)
{
	*table_address = 0;
	return AE_SUPPORT;
}

static uacpi_status update_initial_table_array(void)
{
	uacpi_size i, count;
	uacpi_table_info info;

	mutex_lock(&shim_table_array_lock);

	if (!shim_table_array) {
		mutex_unlock(&shim_table_array_lock);
		return UACPI_STATUS_OK;
	}

	count = uacpi_table_count();
	if (shim_table_array_length < count) {
		mutex_unlock(&shim_table_array_lock);
		return UACPI_STATUS_OUT_OF_MEMORY;
	}

	for (i = 0; i < count; i++) {
		if (uacpi_table_info_get_by_index(i, &info) != UACPI_STATUS_OK)
			continue;

		switch (info.origin) {
		case UACPI_TABLE_ORIGIN_FIRMWARE_PHYSICAL:
			shim_table_array[i].address = info.phys_addr;
			shim_table_array[i].pointer = UACPI_NULL;
			break;
		case UACPI_TABLE_ORIGIN_FIRMWARE_VIRTUAL:
			shim_table_array[i].address = 0;
			shim_table_array[i].pointer = info.virt_addr;
			break;
		case UACPI_TABLE_ORIGIN_HOST_PHYSICAL:
			shim_table_array[i].address = info.phys_addr;
			shim_table_array[i].pointer = UACPI_NULL;
			break;
		case UACPI_TABLE_ORIGIN_HOST_VIRTUAL:
			shim_table_array[i].address = 0;
			shim_table_array[i].pointer = info.virt_addr;
			break;
		}

		shim_table_array[i].length = info.size;
		memcpy(shim_table_array[i].signature.ascii, info.signature,
		       sizeof(info.signature));
		shim_table_array[i].owner_id = 0;

		shim_table_array[i].flags = 0;
		if (info.flags & UACPI_TABLE_LOADED)
			shim_table_array[i].flags |= ACPI_TABLE_IS_LOADED;
		if (!(info.flags & UACPI_TABLE_CSUM_BAD))
			shim_table_array[i].flags |= ACPI_TABLE_IS_VERIFIED;

		shim_table_array[i].validation_count = info.reference_count;
	}

	mutex_unlock(&shim_table_array_lock);
	return UACPI_STATUS_OK;
}

acpi_status ACPI_INIT_FUNCTION
acpi_initialize_tables(struct acpi_table_desc *initial_table_array,
		       u32 initial_table_count, u8 allow_resize)
{
	uacpi_status st;
	struct acpi_fadt *ufadt;
	uacpi_bool reduced;

	st = uacpi_setup_early_table_access(shim_early_table_buffer,
					    sizeof(shim_early_table_buffer));
	if (st != UACPI_STATUS_OK)
		return uacpi_convert_status(st);

	if (initial_table_array) {
		mutex_lock(&shim_table_array_lock);
		shim_table_array = initial_table_array;
		shim_table_array_length = initial_table_count;
		mutex_unlock(&shim_table_array_lock);

		st = update_initial_table_array();
		if (st != UACPI_STATUS_OK) {
			uacpi_state_reset();
			return uacpi_convert_status(st);
		}
	}

	if (uacpi_table_fadt(&ufadt) == UACPI_STATUS_OK)
		memcpy(&acpi_gbl_FADT, ufadt, sizeof(acpi_gbl_FADT));

	uacpi_is_platform_reduced_hardware(&reduced);
	acpi_gbl_reduced_hardware = reduced;
	return AE_OK;
}

acpi_status ACPI_INIT_FUNCTION acpi_reallocate_root_table(void)
{
	mutex_lock(&shim_table_array_lock);

	shim_table_array = NULL;
	shim_table_array_length = 0;

	mutex_unlock(&shim_table_array_lock);
	return AE_OK;
}

acpi_status ACPI_INIT_FUNCTION
acpi_install_table(struct acpi_table_header *table)
{
	uacpi_status st;

	st = uacpi_table_install(table, UACPI_NULL);
	if (st != UACPI_STATUS_OK)
		return uacpi_convert_status(st);

	st = update_initial_table_array();
	return uacpi_convert_status(st);
}

acpi_status ACPI_INIT_FUNCTION
acpi_install_physical_table(acpi_physical_address address)
{
	uacpi_status st;

	st = uacpi_table_install_physical(address, UACPI_NULL);
	if (st != UACPI_STATUS_OK)
		return uacpi_convert_status(st);

	st = update_initial_table_array();
	return uacpi_convert_status(st);
}

acpi_status acpi_load_table(struct acpi_table_header *table, u32 *table_idx)
{
	uacpi_status st;
	uacpi_table table_descriptor;

	st = uacpi_table_install(table, &table_descriptor);
	if (st != UACPI_STATUS_OK)
		return uacpi_convert_status(st);

	st = uacpi_table_load(table_descriptor.index);
	if (st != UACPI_STATUS_OK) {
		// To ensure a proper initial table array view of uACPI's tables,
		// it must be updated here since a table has been installed and has
		// not been uninstalled. However, any errors are ignored since the
		// error on table load is more important for correct error handling.
		update_initial_table_array();
		return uacpi_convert_status(st);
	}

	st = update_initial_table_array();
	if (st != UACPI_STATUS_OK)
		return uacpi_convert_status(st);

	if (table_idx)
		*table_idx = table_descriptor.index;

	// TODO: Handle uninstalling table after error.
	return uacpi_convert_status(st);
}

acpi_status acpi_unload_parent_table(acpi_handle object)
{
	// TOOD: Handle unloading parent tables.
	return AE_SUPPORT;
}

acpi_status acpi_unload_table(u32 table_index)
{
	// TODO: Handle unloading tables.
	return AE_SUPPORT;
}

acpi_status acpi_get_table(char *signature, u32 instance,
			   struct acpi_table_header **out_table)
{
	uacpi_status st;
	uacpi_table table;

	if (!signature || !out_table)
		return AE_BAD_PARAMETER;

	st = uacpi_table_find_nth_by_signature(signature, (uacpi_size)instance,
					       &table);
	if (st != UACPI_STATUS_OK)
		return uacpi_convert_status(st);

	*out_table = table.ptr;
	return AE_OK;
}

void acpi_put_table(struct acpi_table_header *table)
{
	uacpi_status st;
	uacpi_table table_descriptor;
	uacpi_size i, count;

	count = uacpi_table_count();
	for (i = 0; i < count; i++) {
		st = uacpi_table_get_by_index(i, &table_descriptor);
		if (st != UACPI_STATUS_OK) {
			continue;
		}

		if (table_descriptor.ptr != table) {
			uacpi_table_unref_by_index(i);
			continue;
		}

		// Two unrefs are performed since one ref was obtained through
		// `uacpi_table_get_by_index` and the other unref is required
		// to implement `acpi_put_table`'s semantics.
		uacpi_table_unref_by_index(i);
		uacpi_table_unref_by_index(i);
		return;
	}
}

acpi_status acpi_get_table_header(char *signature, u32 instance,
				  struct acpi_table_header *out_table_header)
{
	uacpi_size i, count;

	if (!signature || !out_table_header)
		return AE_BAD_PARAMETER;

	count = uacpi_table_count();
	for (i = 0; i < count; i++) {
		uacpi_table_info info;

		if (uacpi_table_info_get_by_index(i, &info) != UACPI_STATUS_OK)
			continue;

		if (memcmp(info.signature, signature, sizeof(info.signature)))
			continue;

		if (instance != 0) {
			instance--;
			continue;
		}

		switch (info.origin) {
		case UACPI_TABLE_ORIGIN_FIRMWARE_PHYSICAL:
		case UACPI_TABLE_ORIGIN_HOST_PHYSICAL:
			void *p = uacpi_kernel_map(info.phys_addr,
						   sizeof(*out_table_header));
			if (p == UACPI_MAP_FAILED)
				return AE_NO_MEMORY;

			memcpy(out_table_header, p, sizeof(*out_table_header));
			uacpi_kernel_unmap(p, sizeof(*out_table_header));
			break;
		case UACPI_TABLE_ORIGIN_FIRMWARE_VIRTUAL:
		case UACPI_TABLE_ORIGIN_HOST_VIRTUAL:
			memcpy(out_table_header, info.virt_addr,
			       sizeof(*out_table_header));
			break;
		}
		return AE_OK;
	}

	return AE_NOT_FOUND;
}

acpi_status acpi_get_table_by_index(u32 table_index,
				    struct acpi_table_header **out_table)
{
	uacpi_status st;
	uacpi_table table;

	if (!out_table)
		return AE_BAD_PARAMETER;

	if (table_index >= uacpi_table_count())
		return AE_BAD_PARAMETER;

	st = uacpi_table_get_by_index(table_index, &table);
	if (st != UACPI_STATUS_OK)
		return uacpi_convert_status(st);

	*out_table = table.ptr;
	return AE_OK;
}

static DEFINE_MUTEX(shim_interface_handler_lock);
static acpi_table_handler shim_interface_handler;
static void *shim_interface_handler_context;

static uacpi_table_installation_disposition
shim_table_handler_implementation(struct acpi_sdt_hdr *hdr,
				  uacpi_u64 *out_override_address)
{
	(void)out_override_address;

	mutex_lock(&shim_interface_handler_lock);

	shim_interface_handler(ACPI_TABLE_EVENT_LOAD, hdr,
			       shim_interface_handler_context);

	mutex_unlock(&shim_interface_handler_lock);
	return UACPI_TABLE_INSTALLATION_DISPOSITON_ALLOW;
}

acpi_status acpi_install_table_handler(acpi_table_handler handler,
				       void *context)
{
	uacpi_status st;

	acpi_os_printf(
		"ACPI: WARN: acpi_install_table_handler only provides install events\n");
	if (!handler)
		return AE_BAD_PARAMETER;

	mutex_lock(&shim_interface_handler_lock);

	if (shim_interface_handler) {
		mutex_unlock(&shim_interface_handler_lock);
		return AE_ALREADY_EXISTS;
	}

	st = uacpi_set_table_installation_handler(
		shim_table_handler_implementation);
	if (st != UACPI_STATUS_OK) {
		mutex_unlock(&shim_interface_handler_lock);
		return uacpi_convert_status(st);
	}

	shim_interface_handler = handler;
	shim_interface_handler_context = context;

	mutex_unlock(&shim_interface_handler_lock);
	return uacpi_convert_status(st);
}

acpi_status acpi_remove_table_handler(acpi_table_handler handler)
{
	uacpi_status st;

	if (!handler)
		return AE_BAD_PARAMETER;

	mutex_lock(&shim_interface_handler_lock);

	if (shim_interface_handler == NULL) {
		mutex_unlock(&shim_interface_handler_lock);
		return AE_NOT_EXIST;
	}

	if (handler != shim_interface_handler) {
		mutex_unlock(&shim_interface_handler_lock);
		return AE_BAD_PARAMETER;
	}

	st = uacpi_set_table_installation_handler(UACPI_NULL);

	mutex_unlock(&shim_interface_handler_lock);
	return uacpi_convert_status(st);
}

void shim_tables_initialize(void)
{
	mutex_lock(&shim_interface_handler_lock);

	shim_interface_handler = NULL;
	shim_interface_handler = NULL;

	mutex_unlock(&shim_interface_handler_lock);
}

void shim_tables_deinitialize(void)
{
	mutex_lock(&shim_table_array_lock);

	shim_table_array = NULL;
	shim_table_array_length = 0;

	mutex_unlock(&shim_table_array_lock);

	mutex_lock(&shim_interface_handler_lock);

	shim_interface_handler = NULL;
	shim_interface_handler = NULL;

	mutex_unlock(&shim_interface_handler_lock);
}
