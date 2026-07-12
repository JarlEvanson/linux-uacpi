#include <linux/acpi.h>

#include <uacpi/kernel_api.h>
#include <uacpi/types.h>
#include <uacpi/uacpi.h>
#include <uacpi/utilities.h>

#include "shim_shared.h"

static void free_uacpi_object_array(uacpi_object_array *array)
{
	uacpi_size i;

	if (!array->objects)
		goto finalize;

	for (i = 0; i < array->count; i++)
		uacpi_object_unref(array->objects[i]);

	uacpi_kernel_free(array->objects);

finalize:
	array->objects = UACPI_NULL;
	array->count = 0;
}

static acpi_status acpi_to_uacpi_object(union acpi_object *in,
					uacpi_object **out);

static acpi_status acpi_to_uacpi_package(union acpi_object *in,
					 uacpi_object **out)
{
	uacpi_object_array array;
	uacpi_size size;
	acpi_status acpi_st;

	array.objects = UACPI_NULL;
	array.count = 0;

	if (!in->package.count)
		goto exit_success;

	size = array_size(in->package.count, sizeof(uacpi_object *));
	if (size == SIZE_MAX) {
		acpi_st = AE_NO_MEMORY;
		goto exit_error;
	}

	array.objects = uacpi_kernel_alloc(size);
	if (!array.objects) {
		acpi_st = AE_NO_MEMORY;
		goto exit_error;
	}

	for (array.count = 0; array.count < in->package.count; array.count++) {
		acpi_st =
			acpi_to_uacpi_object(&in->package.elements[array.count],
					     &array.objects[array.count]);
		if (ACPI_FAILURE(acpi_st))
			goto exit_error;
	}

exit_success:
	*out = uacpi_object_create_package(array);
	free_uacpi_object_array(&array);
	return AE_OK;

exit_error:
	free_uacpi_object_array(&array);
	return acpi_st;
}

static acpi_status acpi_to_uacpi_object(union acpi_object *in,
					uacpi_object **out)
{
	uacpi_data_view data_view;
	acpi_status acpi_st;

	switch (in->type) {
	case ACPI_TYPE_ANY:
		*out = uacpi_object_create_uninitialized();
		break;
	case ACPI_TYPE_INTEGER:
		*out = uacpi_object_create_integer(in->integer.value);
		break;
	case ACPI_TYPE_STRING:
		data_view.text = in->string.pointer;
		data_view.length = in->string.length + 1;
		*out = uacpi_object_create_string(data_view);
		break;
	case ACPI_TYPE_BUFFER:
		data_view.bytes = in->buffer.pointer;
		data_view.length = in->buffer.length;
		*out = uacpi_object_create_buffer(data_view);
		break;
	case ACPI_TYPE_PACKAGE:
		acpi_st = acpi_to_uacpi_package(in, out);
		if (ACPI_FAILURE(acpi_st))
			return acpi_st;
		break;
	default:
		shim_warn("acpi_to_uacpi_object does not support type %x",
			  in->type);
		return AE_NOT_IMPLEMENTED;
	}

	if (!*out)
		return AE_NO_MEMORY;
	return AE_OK;
}

static acpi_status acpi_to_uacpi_object_list(struct acpi_object_list *in,
					     uacpi_object_array *out)
{
	uacpi_size size;
	acpi_status acpi_st;

	if (!out)
		return AE_BAD_PARAMETER;

	out->objects = UACPI_NULL;
	out->count = 0;

	if (!in || !in->count)
		return AE_OK;

	size = array_size(in->count, sizeof(uacpi_object *));
	if (size == SIZE_MAX) {
		acpi_st = AE_NO_MEMORY;
		goto exit_error;
	}

	out->objects = uacpi_kernel_alloc(size);
	if (!out->objects)
		return AE_NO_MEMORY;

	for (out->count = 0; out->count < in->count; out->count++) {
		acpi_st = acpi_to_uacpi_object(&in->pointer[out->count],
					       &out->objects[out->count]);
		if (ACPI_FAILURE(acpi_st))
			goto exit_error;
	}

	return UACPI_STATUS_OK;

exit_error:
	free_uacpi_object_array(out);
	return acpi_st;
}

static acpi_status shim_compute_required_extra_size(uacpi_object *object,
						    uacpi_size *out_size)
{
	uacpi_data_view data_view;
	acpi_status acpi_st;
	uacpi_status st;

	switch (uacpi_object_get_type(object)) {
	case UACPI_OBJECT_INTEGER:
		*out_size = 0;
		break;
	case UACPI_OBJECT_STRING:
		acpi_st = uacpi_object_get_string(object, &data_view);
		if (st != UACPI_STATUS_OK)
			return uacpi_to_acpi_status(st);

		*out_size = ALIGN(data_view.length ? data_view.length : 1,
				  sizeof(void *));
		break;
	case UACPI_OBJECT_BUFFER:
		st = uacpi_object_get_buffer(object, &data_view);
		if (st != UACPI_STATUS_OK)
			return uacpi_to_acpi_status(st);

		*out_size = ALIGN(data_view.length, sizeof(void *));
		break;
	case UACPI_OBJECT_POWER_RESOURCE:
		*out_size = 0;
		break;
	case UACPI_OBJECT_PROCESSOR:
		*out_size = 0;
		break;
	default:
		shim_warn(
			"shim_compute_required_extra_size does not support type %s\n",
			uacpi_object_type_to_string(
				uacpi_object_get_type(object)));
		return AE_NOT_IMPLEMENTED;
	}

	return AE_OK;
}

static acpi_status shim_write_object(uacpi_object *in, union acpi_object *out,
				     u8 **data_pointer,
				     uacpi_namespace_node *node)
{
	u64 integer;
	uacpi_data_view data_view;
	uacpi_power_resource_info power_info;
	uacpi_processor_info processor_info;
	uacpi_status st;

	switch (uacpi_object_get_type(in)) {
	case UACPI_OBJECT_INTEGER:
		st = uacpi_object_get_integer(in, &integer);
		if (st != UACPI_STATUS_OK)
			return uacpi_to_acpi_status(st);

		out->integer.type = ACPI_TYPE_INTEGER;
		out->integer.value = integer;
		break;
	case UACPI_OBJECT_STRING:
		st = uacpi_object_get_string(in, &data_view);
		if (st != UACPI_STATUS_OK)
			return uacpi_to_acpi_status(st);

		out->string.type = ACPI_TYPE_STRING;
		out->string.length = data_view.length ? data_view.length - 1 :
							0;
		out->string.pointer = *data_pointer;
		if (out->string.length)
			memcpy(out->string.pointer, data_view.const_text,
			       out->string.length);
		out->string.pointer[out->string.length] = '\0';

		*data_pointer += ALIGN(data_view.length ? data_view.length : 1,
				       sizeof(void *));
		break;
	case UACPI_OBJECT_BUFFER:
		st = uacpi_object_get_buffer(in, &data_view);
		if (st != UACPI_STATUS_OK)
			return uacpi_to_acpi_status(st);

		out->buffer.type = ACPI_TYPE_BUFFER;
		out->buffer.length = data_view.length;
		out->buffer.pointer = *data_pointer;
		memcpy(out->buffer.pointer, data_view.const_bytes,
		       out->buffer.length);

		*data_pointer += ALIGN(data_view.length, sizeof(void *));
		break;
	case UACPI_OBJECT_POWER_RESOURCE:
		st = uacpi_object_get_power_resource_info(in, &power_info);
		if (st != UACPI_STATUS_OK)
			return uacpi_to_acpi_status(st);

		out->power_resource.type = ACPI_TYPE_POWER;
		out->power_resource.system_level = power_info.system_level;
		out->power_resource.resource_order = power_info.resource_order;
		break;
	case UACPI_OBJECT_PROCESSOR:
		st = uacpi_object_get_processor_info(in, &processor_info);
		if (st != UACPI_STATUS_OK)
			return uacpi_to_acpi_status(st);

		out->processor.type = ACPI_TYPE_PROCESSOR;
		out->processor.proc_id = processor_info.id;
		out->processor.pblk_address = processor_info.block_address;
		out->processor.pblk_length = processor_info.block_length;
		break;
	default:
		shim_warn(
			"shim_write_object does not support %s",
			uacpi_object_type_to_string(uacpi_object_get_type(in)));
		return AE_NOT_IMPLEMENTED;
	}

	return AE_OK;
}

static acpi_status uacpi_to_acpi_object(uacpi_object *in,
					struct acpi_buffer *out,
					uacpi_namespace_node *node)
{
	uacpi_size required_size;
	u8 *extra_data_pointer;
	acpi_status acpi_st;

	required_size = 0;
	acpi_st = shim_compute_required_extra_size(in, &required_size);
	if (ACPI_FAILURE(acpi_st))
		return acpi_st;

	if (check_add_overflow(required_size, sizeof(union acpi_object),
			       &required_size))
		return AE_NO_MEMORY;

	if (out->length == ACPI_ALLOCATE_BUFFER) {
		out->pointer = uacpi_kernel_alloc(required_size);
		if (!out->pointer)
			return AE_NO_MEMORY;
	} else {
		if (out->length < required_size) {
			out->length = required_size;
			return AE_BUFFER_OVERFLOW;
		}

		memset(out->pointer, 0, required_size);
	}

	extra_data_pointer = (u8 *)out->pointer + sizeof(union acpi_object);
	acpi_st =
		shim_write_object(in, out->pointer, &extra_data_pointer, node);
	if (ACPI_FAILURE(acpi_st) && out->length == ACPI_ALLOCATE_BUFFER)
		uacpi_kernel_free(out->pointer);

	out->length = required_size;
	return acpi_st;
}

acpi_status acpi_evaluate_object(acpi_handle handle, acpi_string pathname,
				 struct acpi_object_list *external_params,
				 struct acpi_buffer *return_buffer)
{
	uacpi_namespace_node *node;
	uacpi_object_array args;
	uacpi_object *ret = UACPI_NULL;
	acpi_status acpi_st;
	uacpi_status st;

	acpi_st = acpi_to_uacpi_object_list(external_params, &args);
	if (ACPI_FAILURE(acpi_st))
		return acpi_st;

	if (handle) {
		node = uacpi_node_from_handle(handle);
	} else {
		node = UACPI_NULL;
	}

	st = uacpi_eval(node, pathname, &args,
			return_buffer ? &ret : UACPI_NULL);
	free_uacpi_object_array(&args);

	if (st != UACPI_STATUS_OK)
		return uacpi_to_acpi_status(st);

	if (return_buffer && ret) {
		acpi_st = uacpi_to_acpi_object(ret, return_buffer, node);
		uacpi_object_unref(ret);

		return acpi_st;
	}

	return AE_OK;
}

acpi_status acpi_evaluate_object_typed(acpi_handle handle, acpi_string pathname,
				       struct acpi_object_list *external_params,
				       struct acpi_buffer *return_buffer,
				       acpi_object_type return_type)
{
	union acpi_object *returned_object;
	bool requested_alloc;
	acpi_status acpi_st;

	if (!return_buffer)
		return AE_BAD_PARAMETER;

	requested_alloc = return_buffer->length == ACPI_ALLOCATE_BUFFER;

	acpi_st = acpi_evaluate_object(handle, pathname, external_params,
				       return_buffer);
	if (acpi_st != AE_OK)
		return acpi_st;

	returned_object = return_buffer->pointer;
	if (returned_object && returned_object->type != return_type) {
		if (requested_alloc) {
			uacpi_kernel_free(return_buffer->pointer);
			return_buffer->pointer = NULL;
			return_buffer->length = 0;
		}
		return AE_TYPE;
	}

	return AE_OK;
}

acpi_status acpi_get_type(acpi_handle handle, acpi_object_type *ret_type)
{
	uacpi_namespace_node *node;
	uacpi_object_type object_type;
	uacpi_status st;

	if (!ret_type)
		return AE_BAD_PARAMETER;

	if (handle == ACPI_ROOT_OBJECT) {
		*ret_type = ACPI_TYPE_ANY;
		return AE_OK;
	}

	node = uacpi_node_from_handle(handle);
	st = uacpi_namespace_node_type(node, &object_type);
	if (st != UACPI_STATUS_OK)
		return uacpi_to_acpi_status(st);

	return uacpi_to_acpi_type(object_type, ret_type);
}

static void shim_copy_id(struct acpi_pnp_device_id *dst, uacpi_id_string *src,
			 char **strings)
{
	dst->string = *strings;
	dst->length = src->size;

	memcpy(dst->string, src->value, src->size);

	*strings += src->size;
}

acpi_status acpi_get_object_info(acpi_handle handle,
				 struct acpi_device_info **return_buffer)
{
	uacpi_namespace_node *node;
	uacpi_namespace_node_info *uacpi_info;
	struct acpi_device_info *acpi_info;
	char *strings;
	u32 i;
	uacpi_size total_size;
	uacpi_status st;

	if (!return_buffer)
		return AE_BAD_PARAMETER;

	node = uacpi_node_from_handle(handle);
	st = uacpi_get_namespace_node_info(node, &uacpi_info);
	if (st != UACPI_STATUS_OK)
		return uacpi_to_acpi_status(st);

	total_size = sizeof(struct acpi_device_info);
	if (uacpi_info->flags & UACPI_NS_NODE_INFO_HAS_HID)
		total_size += uacpi_info->hid.size;

	if (uacpi_info->flags & UACPI_NS_NODE_INFO_HAS_UID)
		total_size += uacpi_info->uid.size;

	if (uacpi_info->flags & UACPI_NS_NODE_INFO_HAS_CID) {
		total_size += uacpi_info->cid.num_ids *
			      sizeof(struct acpi_pnp_device_id);
		for (i = 0; i < uacpi_info->cid.num_ids; i++)
			total_size += uacpi_info->cid.ids[i].size;
	}

	if (uacpi_info->flags & UACPI_NS_NODE_INFO_HAS_CLS)
		total_size += uacpi_info->cls.size;

	acpi_info = uacpi_kernel_alloc(total_size);
	if (!acpi_info) {
		uacpi_free_namespace_node_info(uacpi_info);
		return AE_NO_MEMORY;
	}

	memset(acpi_info, 0, total_size);

	acpi_info->info_size = total_size;
	acpi_info->name = uacpi_namespace_node_name(node).id;

	st = uacpi_to_acpi_type(uacpi_info->type, &acpi_info->type);
	if (st != UACPI_STATUS_OK) {
		uacpi_kernel_free(acpi_info);
		uacpi_free_namespace_node_info(uacpi_info);
		return uacpi_to_acpi_status(st);
	}

	acpi_info->param_count = uacpi_info->num_params;

	acpi_info->valid = 0;
	strings = (char *)acpi_info->compatible_id_list.ids;
	if (uacpi_info->flags & UACPI_NS_NODE_INFO_HAS_CID)
		strings += uacpi_info->cid.num_ids *
			   sizeof(struct acpi_pnp_device_id);

	if (uacpi_info->flags & UACPI_NS_NODE_INFO_HAS_ADR) {
		acpi_info->address = uacpi_info->adr;
		acpi_info->valid |= ACPI_VALID_ADR;
	}

	if (uacpi_info->flags & UACPI_NS_NODE_INFO_HAS_HID) {
		shim_copy_id(&acpi_info->hardware_id, &uacpi_info->hid,
			     &strings);
		acpi_info->valid |= ACPI_VALID_HID;
	}

	if (uacpi_info->flags & UACPI_NS_NODE_INFO_HAS_UID) {
		shim_copy_id(&acpi_info->unique_id, &uacpi_info->uid, &strings);
		acpi_info->valid |= ACPI_VALID_UID;
	}

	if (uacpi_info->flags & UACPI_NS_NODE_INFO_HAS_CID) {
		acpi_info->compatible_id_list.count = uacpi_info->cid.num_ids;
		acpi_info->compatible_id_list.list_size = 0;

		for (i = 0; i < acpi_info->compatible_id_list.count; i++) {
			shim_copy_id(&acpi_info->compatible_id_list.ids[i],
				     &uacpi_info->cid.ids[i], &strings);

			acpi_info->compatible_id_list.list_size +=
				sizeof(struct acpi_pnp_device_id);
			acpi_info->compatible_id_list.list_size +=
				uacpi_info->cid.ids[i].size;
		}

		acpi_info->valid |= ACPI_VALID_CID;
	}

	if (uacpi_info->flags & UACPI_NS_NODE_INFO_HAS_CLS) {
		shim_copy_id(&acpi_info->class_code, &uacpi_info->cls,
			     &strings);
		acpi_info->valid |= ACPI_VALID_CLS;
	}

	if (uacpi_info->flags & UACPI_NS_NODE_INFO_HAS_SXD) {
		memcpy(&acpi_info->highest_dstates[0], &uacpi_info->sxd[0],
		       sizeof(acpi_info->highest_dstates));
		acpi_info->valid |= ACPI_VALID_SXDS;
	}

	if (uacpi_info->flags & UACPI_NS_NODE_INFO_HAS_SXW) {
		memcpy(&acpi_info->lowest_dstates[0], &uacpi_info->sxw[0],
		       sizeof(acpi_info->highest_dstates));
		acpi_info->valid |= ACPI_VALID_SXWS;
	}

	uacpi_free_namespace_node_info(uacpi_info);
	*return_buffer = acpi_info;
	return AE_OK;
}

acpi_status acpi_acquire_mutex(acpi_handle handle, acpi_string pathname,
			       u16 timeout)
{
	shim_warn("acpi_release_mutex is not implemented by uACPI");
	return AE_NOT_IMPLEMENTED;
}

acpi_status acpi_release_mutex(acpi_handle handle, acpi_string pathname)
{
	shim_warn("acpi_release_mutex is not implemented by uACPI");
	return AE_NOT_IMPLEMENTED;
}
