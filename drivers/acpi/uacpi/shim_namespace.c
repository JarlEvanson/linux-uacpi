#include <linux/acpi.h>
#include <linux/rhashtable.h>

#include <uacpi/kernel_api.h>
#include <uacpi/namespace.h>
#include <uacpi/types.h>
#include <uacpi/utilities.h>

#include "shim_shared.h"

acpi_status acpi_get_handle(acpi_handle parent, const char *pathname,
			    acpi_handle *ret_handle)
{
	uacpi_namespace_node *node, *out;
	uacpi_status st;

	if (!pathname || !ret_handle)
		return AE_BAD_PARAMETER;

	node = uacpi_node_from_handle(parent);
	st = uacpi_namespace_node_find(node, pathname, &out);
	if (st != UACPI_STATUS_OK)
		return uacpi_to_acpi_status(st);

	*ret_handle = acpi_handle_from_node(out);
	return AE_OK;
}

acpi_status acpi_get_name(acpi_handle handle, u32 name_type,
			  struct acpi_buffer *buffer)
{
	uacpi_namespace_node *node;
	char name_buffer[ACPI_NAMESEG_SIZE + 1];
	char *ptr;
	size_t len;
	bool allocated;

	if (name_type > ACPI_NAME_TYPE_MAX)
		return AE_BAD_PARAMETER;

	if (!buffer)
		return AE_BAD_PARAMETER;

	node = uacpi_node_from_handle(handle);
	switch (name_type) {
	case ACPI_FULL_PATHNAME:
		ptr = (char *)uacpi_namespace_node_generate_absolute_path(node);
		if (!ptr)
			return AE_NO_MEMORY;

		allocated = true;
		break;
	case ACPI_SINGLE_NAME:
		uacpi_object_name name = uacpi_namespace_node_name(node);

		memcpy(name_buffer, name.text, ACPI_NAMESEG_SIZE);
		name_buffer[ACPI_NAMESEG_SIZE] = '\0';
		ptr = name_buffer;

		allocated = false;
		break;
	default:
		return AE_SUPPORT;
	}

	len = strlen(ptr) + 1;

	if (buffer->length == ACPI_ALLOCATE_BUFFER) {
		buffer->pointer = uacpi_kernel_alloc(len);
		if (!buffer->pointer) {
			if (allocated)
				uacpi_free_absolute_path(ptr);
			return AE_NO_MEMORY;
		}

		buffer->length = len;
	} else if (buffer->length < len) {
		buffer->length = len;

		if (allocated)
			uacpi_free_absolute_path(ptr);
		return AE_BUFFER_OVERFLOW;
	} else if (!buffer->pointer) {
		if (allocated)
			uacpi_free_absolute_path(ptr);
		return AE_BAD_PARAMETER;
	}

	memcpy(buffer->pointer, ptr, len);
	buffer->length = len;

	if (allocated)
		uacpi_free_absolute_path(ptr);
	return AE_OK;
}

acpi_status acpi_get_parent(acpi_handle handle, acpi_handle *ret_handle)
{
	uacpi_namespace_node *node, *parent;

	if (!ret_handle)
		return AE_BAD_PARAMETER;

	node = uacpi_node_from_handle(handle);
	parent = uacpi_namespace_node_parent(node);
	if (!parent)
		return AE_NULL_ENTRY;

	*ret_handle = acpi_handle_from_node(parent);
	return AE_OK;
}

acpi_status acpi_get_next_object(acpi_object_type type, acpi_handle parent,
				 acpi_handle child, acpi_handle *ret_handle)
{
	uacpi_object_type_bits object_type;
	uacpi_namespace_node *parent_node, *child_node;
	acpi_status acpi_st;
	uacpi_status st;

	acpi_st = acpi_type_to_uacpi_type_bits(type, &object_type);
	if (ACPI_FAILURE(acpi_st))
		return acpi_st;

	parent_node = uacpi_node_from_handle(parent);
	if (child) {
		child_node = uacpi_node_from_handle(child);
	} else {
		child_node = UACPI_NULL;
	}

	st = uacpi_namespace_node_next_typed(parent_node, &child_node,
					     object_type);
	if (st != UACPI_STATUS_OK)
		return uacpi_to_acpi_status(st);

	if (ret_handle)
		*ret_handle = acpi_handle_from_node(child);
	return AE_OK;
}

acpi_status acpi_install_method(u8 *buffer)
{
	BUG();
}

struct shim_walk_namespace_context {
	acpi_walk_callback descending;
	acpi_walk_callback ascending;

	void *context;
	void **return_value;

	acpi_status return_status;
};

static uacpi_iteration_decision
shim_descending_callback(void *user, uacpi_namespace_node *node,
			 uacpi_u32 node_depth)
{
	struct shim_walk_namespace_context *context;
	acpi_handle handle;
	acpi_status acpi_st;

	context = user;
	if (context->return_status != AE_OK)
		return UACPI_ITERATION_DECISION_BREAK;

	if (!context->descending)
		return UACPI_ITERATION_DECISION_CONTINUE;

	handle = acpi_handle_from_node(node);
	acpi_st = context->descending(node, node_depth, context->context,
				      context->return_value);
	switch (acpi_st) {
	case AE_OK:
		return UACPI_ITERATION_DECISION_CONTINUE;
	case AE_CTRL_DEPTH:
		return UACPI_ITERATION_DECISION_NEXT_PEER;
	default:
		context->return_status = acpi_st;
		return UACPI_ITERATION_DECISION_BREAK;
	}
}

static uacpi_iteration_decision
shim_ascending_callback(void *user, uacpi_namespace_node *node,
			uacpi_u32 node_depth)
{
	struct shim_walk_namespace_context *context;
	acpi_handle handle;
	acpi_status acpi_st;

	context = user;
	if (context->return_status != AE_OK)
		return UACPI_ITERATION_DECISION_BREAK;

	if (!context->ascending)
		return UACPI_ITERATION_DECISION_CONTINUE;

	handle = acpi_handle_from_node(node);
	acpi_st = context->descending(node, node_depth, context->context,
				      context->return_value);
	switch (acpi_st) {
	case AE_OK:
		return UACPI_ITERATION_DECISION_CONTINUE;
	case AE_CTRL_DEPTH:
		return UACPI_ITERATION_DECISION_NEXT_PEER;
	default:
		context->return_status = acpi_st;
		return UACPI_ITERATION_DECISION_BREAK;
	}
}

acpi_status acpi_walk_namespace(acpi_object_type type, acpi_handle start_object,
				u32 max_depth,
				acpi_walk_callback descending_callback,
				acpi_walk_callback ascending_callback,
				void *context, void **return_value)
{
	uacpi_namespace_node *node;
	uacpi_object_type_bits type_mask;
	struct shim_walk_namespace_context walk_namespace_context;
	acpi_status acpi_st;
	uacpi_status st;

	node = uacpi_node_from_handle(start_object);

	acpi_st = acpi_type_to_uacpi_type_bits(type, &type_mask);
	if (ACPI_FAILURE(acpi_st))
		return acpi_st;

	walk_namespace_context.descending = descending_callback;
	walk_namespace_context.ascending = ascending_callback;
	walk_namespace_context.context = context;
	walk_namespace_context.return_value = return_value;
	walk_namespace_context.return_status = AE_OK;

	st = uacpi_namespace_for_each_child(node, shim_descending_callback,
					    shim_ascending_callback, type_mask,
					    max_depth, &walk_namespace_context);
	if (st != UACPI_STATUS_OK)
		return uacpi_to_acpi_status(st);

	if (walk_namespace_context.return_status == AE_CTRL_TERMINATE)
		walk_namespace_context.return_status = AE_OK;

	return walk_namespace_context.return_status;
}

struct shim_walk_devices_context {
	acpi_walk_callback user_function;

	void *context;
	void **return_value;

	acpi_status return_status;
};

static uacpi_iteration_decision
shim_get_devices_callback(void *user, uacpi_namespace_node *node,
			  uacpi_u32 node_depth)
{
	struct shim_walk_devices_context *context;
	acpi_handle handle;
	acpi_status acpi_st;

	context = user;
	if (!context->return_status != AE_OK)
		return UACPI_ITERATION_DECISION_BREAK;

	handle = acpi_handle_from_node(node);
	acpi_st = context->user_function(node, node_depth, context->context,
					 context->return_value);
	switch (acpi_st) {
	case AE_OK:
		return UACPI_ITERATION_DECISION_CONTINUE;
	case AE_CTRL_DEPTH:
		return UACPI_ITERATION_DECISION_NEXT_PEER;
	default:
		context->return_status = acpi_st;
		return UACPI_ITERATION_DECISION_BREAK;
	}
}

acpi_status acpi_get_devices(const char *HID, acpi_walk_callback user_function,
			     void *context, void **return_value)
{
	struct shim_walk_devices_context walk_devices_context;
	uacpi_status st;

	if (!user_function)
		return AE_BAD_PARAMETER;

	walk_devices_context.user_function = user_function;
	walk_devices_context.context = context;
	walk_devices_context.return_value = return_value;
	walk_devices_context.return_status = AE_OK;

	if (HID) {
		st = uacpi_find_devices(HID, shim_get_devices_callback,
					&walk_devices_context);
	} else {
		st = uacpi_namespace_for_each_child(
			uacpi_namespace_root(), shim_get_devices_callback,
			UACPI_NULL, UACPI_OBJECT_DEVICE_BIT,
			UACPI_MAX_DEPTH_ANY, &walk_devices_context);
	}

	if (st != UACPI_STATUS_OK)
		return uacpi_to_acpi_status(st);

	if (walk_devices_context.return_status == AE_CTRL_TERMINATE)
		walk_devices_context.return_status = AE_OK;

	return walk_devices_context.return_status;
}

struct shim_attached_data_key {
	acpi_handle handle;
	acpi_object_handler handler;
};

struct shim_attached_data_node {
	struct rhash_head head;
	struct shim_attached_data_key key;
	void *data;
};

static struct rhashtable shim_attached_data_ht;

static const struct rhashtable_params shim_attached_data_ht_params = {
	.head_offset = offsetof(struct shim_attached_data_node, head),
	.key_offset = offsetof(struct shim_attached_data_node, key),
	.key_len = sizeof(struct shim_attached_data_key),
	.automatic_shrinking = true,
};

acpi_status acpi_attach_data(acpi_handle obj_handle,
			     acpi_object_handler handler, void *data)
{
	struct shim_attached_data_node *data_node;
	struct shim_attached_data_key key;
	int err;

	if (!obj_handle || !handler || !data)
		return AE_BAD_PARAMETER;

	data_node = uacpi_kernel_alloc(sizeof(*data_node));
	if (!data)
		return AE_NO_MEMORY;

	key.handle = obj_handle;
	key.handler = handler;

	data_node->key = key;
	data_node->data = data;

	err = rhashtable_lookup_insert_fast(&shim_attached_data_ht,
					    &data_node->head,
					    shim_attached_data_ht_params);
	if (err) {
		uacpi_kernel_free(data_node);
		if (err == -EEXIST)
			return AE_ALREADY_EXISTS;
		return AE_ERROR;
	}

	return AE_OK;
}

acpi_status acpi_detach_data(acpi_handle obj_handle,
			     acpi_object_handler handler)
{
	struct shim_attached_data_node *data_node;
	struct shim_attached_data_key key;
	int err;

	if (!obj_handle || !handler)
		return AE_BAD_PARAMETER;

	key.handle = obj_handle;
	key.handler = handler;

	data_node = rhashtable_lookup_fast(&shim_attached_data_ht, &key,
					   shim_attached_data_ht_params);
	if (!data_node)
		return AE_NOT_FOUND;

	err = rhashtable_remove_fast(&shim_attached_data_ht, &data_node->head,
				     shim_attached_data_ht_params);
	if (err)
		return AE_ERROR;

	uacpi_kernel_free(data_node);
	return AE_OK;
}

acpi_status acpi_get_data_full(acpi_handle obj_handle,
			       acpi_object_handler handler, void **data,
			       void (*callback)(void *))
{
	struct shim_attached_data_node *data_node;
	struct shim_attached_data_key key;

	if (!obj_handle || !handler || !data)
		return AE_BAD_PARAMETER;

	key.handle = obj_handle;
	key.handler = handler;

	rcu_read_lock();
	data_node = rhashtable_lookup_fast(&shim_attached_data_ht, &key,
					   shim_attached_data_ht_params);
	if (data_node) {
		*data = data_node->data;
		if (callback)
			callback(*data);
	}
	rcu_read_unlock();

	return data_node ? AE_OK : AE_NOT_FOUND;
}

acpi_status acpi_get_data(acpi_handle obj_handle, acpi_object_handler handler,
			  void **data)
{
	return acpi_get_data_full(obj_handle, handler, data, NULL);
}

uacpi_status shim_namespace_initialize(void)
{
	int err;

	err = rhashtable_init(&shim_attached_data_ht,
			      &shim_attached_data_ht_params);
	if (err)
		return UACPI_STATUS_OUT_OF_MEMORY;
	return UACPI_STATUS_OK;
}

static void free_shim_attached_data(void *ptr, void *arg)
{
	uacpi_kernel_free(ptr);
}

void shim_namespace_deinitialize(void)
{
	rhashtable_free_and_destroy(&shim_attached_data_ht,
				    free_shim_attached_data, NULL);
}
