#define ACPI_DEFINE_EXCEPTION_TABLE

#include <linux/acpi.h>

#include <uacpi/namespace.h>
#include <uacpi/opregion.h>
#include <uacpi/internal/types.h>

#include "../acpica/acmacros.h"

#include "shim_shared.h"

struct shim_check_address_range_context {
	uacpi_address_space space_id;
	acpi_physical_address address;
	acpi_size length;
	bool warn;

	u32 counter;
};

static uacpi_iteration_decision
shim_check_address_range(void *user, uacpi_namespace_node *node,
			 uacpi_u32 node_depth)
{
	struct shim_check_address_range_context *context;
	struct uacpi_object *object;
	uacpi_address_space space_id;
	uacpi_u64 offset;
	uacpi_u64 length;

	context = user;

	object = (struct uacpi_object *)node;
	space_id = object->op_region->space;
	offset = object->op_region->offset;
	length = object->op_region->length;

	if (space_id != context->space_id)
		return UACPI_ITERATION_DECISION_CONTINUE;

	if (offset < (context->address + context->length) &&
	    context->address < (offset + length)) {
		context->counter++;

		if (context->warn) {
			const char *path =
				uacpi_namespace_node_generate_absolute_path(
					node);
			shim_warn(
				"%s 0x%8.8llx-0x%8.8llx conflicts with OpRegion 0x%8.8llx-0x%8.8llx (%s)",
				uacpi_address_space_to_string(space_id),
				context->address,
				context->address + context->length - 1, offset,
				length, path);
			uacpi_free_absolute_path(path);
		}
	}

	return UACPI_ITERATION_DECISION_CONTINUE;
}

u32 acpi_check_address_range(acpi_adr_space_type space_id,
			     acpi_physical_address address, acpi_size length,
			     u8 warn)
{
	struct shim_check_address_range_context shim_context;
	acpi_status acpi_st;

	acpi_st = acpi_to_uacpi_address_space(space_id, &shim_context.space_id);
	if (ACPI_FAILURE(acpi_st))
		return 0;

	if (shim_context.space_id != ACPI_ADR_SPACE_SYSTEM_MEMORY &&
	    shim_context.space_id != ACPI_ADR_SPACE_SYSTEM_IO)
		return 0;

	shim_context.address = address;
	shim_context.length = length;
	shim_context.warn = warn != 0;
	shim_context.counter = 0;

	uacpi_namespace_for_each_child(uacpi_namespace_root(),
				       shim_check_address_range, UACPI_NULL,
				       UACPI_OBJECT_OPERATION_REGION_BIT,
				       UACPI_MAX_DEPTH_ANY, &shim_context);

	return shim_context.counter;
}

acpi_status acpi_debug_trace(const char *name, u32 debug_level, u32 debug_layer,
			     u32 flags)
{
	return AE_NOT_IMPLEMENTED;
}

acpi_status acpi_decode_pld_buffer(u8 *in_buffer, acpi_size length,
				   struct acpi_pld_info **return_buffer)
{
	struct acpi_pld_info *pld;
	u32 dword;

	if (!in_buffer || !return_buffer)
		return AE_BAD_PARAMETER;

	if (length < ACPI_PLD_REV1_BUFFER_SIZE)
		return AE_BAD_PARAMETER;

	ACPI_MOVE_32_TO_32(&dword, &in_buffer[0]);
	if (ACPI_PLD_GET_REVISION(&dword) >= 2 &&
	    length < ACPI_PLD_REV2_BUFFER_SIZE)
		return AE_BAD_PARAMETER;

	pld = uacpi_kernel_alloc(sizeof(*pld));
	if (!pld)
		return AE_NO_MEMORY;

	pld->revision = ACPI_PLD_GET_REVISION(&dword);
	pld->ignore_color = ACPI_PLD_GET_IGNORE_COLOR(&dword);
	pld->red = ACPI_PLD_GET_RED(&dword);
	pld->green = ACPI_PLD_GET_GREEN(&dword);
	pld->blue = ACPI_PLD_GET_BLUE(&dword);

	ACPI_MOVE_32_TO_32(&dword, &in_buffer[1]);
	pld->width = ACPI_PLD_GET_WIDTH(&dword);
	pld->height = ACPI_PLD_GET_HEIGHT(&dword);

	ACPI_MOVE_32_TO_32(&dword, &in_buffer[2]);
	pld->user_visible = ACPI_PLD_GET_USER_VISIBLE(&dword);
	pld->dock = ACPI_PLD_GET_DOCK(&dword);
	pld->lid = ACPI_PLD_GET_LID(&dword);
	pld->panel = ACPI_PLD_GET_PANEL(&dword);
	pld->vertical_position = ACPI_PLD_GET_VERTICAL(&dword);
	pld->horizontal_position = ACPI_PLD_GET_HORIZONTAL(&dword);
	pld->shape = ACPI_PLD_GET_SHAPE(&dword);
	pld->group_orientation = ACPI_PLD_GET_ORIENTATION(&dword);
	pld->group_token = ACPI_PLD_GET_TOKEN(&dword);
	pld->group_position = ACPI_PLD_GET_POSITION(&dword);
	pld->bay = ACPI_PLD_GET_BAY(&dword);

	ACPI_MOVE_32_TO_32(&dword, &in_buffer[3]);
	pld->ejectable = ACPI_PLD_GET_EJECTABLE(&dword);
	pld->ospm_eject_required = ACPI_PLD_GET_OSPM_EJECT(&dword);
	pld->cabinet_number = ACPI_PLD_GET_CABINET(&dword);
	pld->card_cage_number = ACPI_PLD_GET_CARD_CAGE(&dword);
	pld->reference = ACPI_PLD_GET_REFERENCE(&dword);
	pld->rotation = ACPI_PLD_GET_ROTATION(&dword);
	pld->order = ACPI_PLD_GET_ORDER(&dword);

	if (pld->revision >= 2) {
		ACPI_MOVE_32_TO_32(&dword, &in_buffer[4]);
		pld->vertical_offset = ACPI_PLD_GET_VERT_OFFSET(&dword);
		pld->horizontal_offset = ACPI_PLD_GET_HORIZ_OFFSET(&dword);
	}

	*return_buffer = pld;
	return AE_OK;
}

const char *acpi_format_exception(acpi_status status)
{
	const struct acpi_exception_info *info_table;
	u32 index, table_size;

	index = status & AE_CODE_MASK;
	if (ACPI_ENV_EXCEPTION(status) || ACPI_SUCCESS(status)) {
		info_table = acpi_gbl_exception_names_env;
		table_size = ARRAY_SIZE(acpi_gbl_exception_names_env);
	} else if (ACPI_PROG_EXCEPTION(status)) {
		info_table = acpi_gbl_exception_names_pgm;
		table_size = ARRAY_SIZE(acpi_gbl_exception_names_pgm);
	} else if (ACPI_TABLE_EXCEPTION(status)) {
		info_table = acpi_gbl_exception_names_tbl;
		table_size = ARRAY_SIZE(acpi_gbl_exception_names_tbl);
	} else if (ACPI_AML_EXCEPTION(status)) {
		info_table = acpi_gbl_exception_names_aml;
		table_size = ARRAY_SIZE(acpi_gbl_exception_names_aml);
	} else if (ACPI_CNTL_EXCEPTION(status)) {
		info_table = acpi_gbl_exception_names_ctrl;
		table_size = ARRAY_SIZE(acpi_gbl_exception_names_ctrl);
	}

	return index >= table_size ? info_table[index].name :
				     "unknown status code";
}

acpi_status acpi_get_statistics(struct acpi_statistics *stats)
{
	return AE_NOT_IMPLEMENTED;
}

acpi_status acpi_get_system_info(struct acpi_buffer *out_buffer)
{
	return AE_NOT_IMPLEMENTED;
}

acpi_status acpi_purge_cached_objects(void)
{
	return AE_OK;
}
