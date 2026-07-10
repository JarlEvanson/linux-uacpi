#include <linux/acpi.h>
#include <linux/once.h>

#include <uacpi/kernel_api.h>
#include <uacpi/resources.h>

#include "shim_shared.h"

#define ACPI_RESOURCE_HEADER offsetof(struct acpi_resource, data)

static u32 uacpi_resource_source_size(const uacpi_resource_source *source)
{
	if (source->string && source->length) {
		return source->length;
	} else {
		return 0;
	}
}

static u32 uacpi_to_acpi_resource_source(const uacpi_resource_source *in,
					 struct acpi_resource_source *out,
					 void *string_location)
{
	u32 length = uacpi_resource_source_size(in);

	out->index = in->index;
	out->string_length = length;
	if (length) {
		// uacpi_resource_source->length include the NUL terminator.
		out->string_ptr = string_location;
		memcpy(out->string_ptr, in->string, length);

	} else {
		out->string_ptr = NULL;
	}

	return length;
}

static acpi_status
uacpi_to_acpi_address_common(const uacpi_resource_address_common *in,
			     struct acpi_resource_address *out)
{
	out->resource_type = in->type;
	out->producer_consumer = in->direction;
	out->decode = in->decode_type;
	out->min_address_fixed = in->fixed_min_address;
	out->max_address_fixed = in->fixed_max_address;

	switch (in->type) {
	case UACPI_RANGE_MEMORY:
		out->info.mem.write_protect = in->attribute.memory.write_status;
		out->info.mem.caching = in->attribute.memory.caching;
		out->info.mem.range_type = in->attribute.memory.range_type;
		out->info.mem.translation = in->attribute.memory.translation;
		break;
	case UACPI_RANGE_IO:
		out->info.io.range_type = in->attribute.io.range_type;
		out->info.io.translation = in->attribute.io.translation;
		out->info.io.translation_type =
			in->attribute.io.translation_type;
		out->info.io.reserved1 = 0;
		break;
	default:
		out->info.type_specific = in->attribute.type_specific;
		break;
	}

	return AE_OK;
}

static acpi_status uacpi_to_acpi_resource(const uacpi_resource *in,
					  struct acpi_resource *out,
					  u32 *length)
{
	acpi_status acpi_st;

	*length = ACPI_RESOURCE_HEADER;
	switch (in->type) {
	case UACPI_RESOURCE_TYPE_IRQ:
		*length += offsetof(struct acpi_resource_irq, interrupt) +
			   in->irq.num_irqs;

		if (out) {
			out->type = ACPI_RESOURCE_TYPE_IRQ;
			DO_ONCE(acpi_os_printf,
				"ACPI: WARN: setting irq.descriptor_length to 0 (unknown translation)\n");
			out->data.irq.descriptor_length = 0;
			out->data.irq.triggering = in->irq.triggering;
			out->data.irq.polarity = in->irq.polarity;
			out->data.irq.shareable = in->irq.sharing;
			out->data.irq.wake_capable = in->irq.wake_capability;
			out->data.irq.interrupt_count = in->irq.num_irqs;
			memcpy(out->data.irq.interrupts, in->irq.irqs,
			       in->irq.num_irqs);
		}
		break;
	case UACPI_RESOURCE_TYPE_EXTENDED_IRQ:
		*length +=
			offsetof(struct acpi_resource_extended_irq, interrupt) +
			in->irq.num_irqs * sizeof(u32) +
			uacpi_resource_source_size(&in->extended_irq.source);

		if (out) {
			out->type = ACPI_RESOURCE_TYPE_EXTENDED_IRQ;
			out->data.extended_irq.producer_consumer =
				in->extended_irq.direction;
			out->data.extended_irq.triggering =
				in->extended_irq.triggering;
			out->data.extended_irq.polarity =
				in->extended_irq.polarity;
			out->data.extended_irq.shareable =
				in->extended_irq.sharing;
			out->data.extended_irq.wake_capable =
				in->extended_irq.wake_capability;
			out->data.extended_irq.interrupt_count =
				in->extended_irq.num_irqs;
			uacpi_to_acpi_resource_source(
				&in->extended_irq.source,
				&out->data.extended_irq.resource_source,
				&out->data.extended_irq.interrupts[0] +
					out->data.extended_irq.interrupt_count);
			memcpy(out->data.extended_irq.interrupts,
			       in->extended_irq.irqs,
			       in->extended_irq.num_irqs * sizeof(u32));
		}
		break;
	case UACPI_RESOURCE_TYPE_IO:
		*length += sizeof(struct acpi_resource_io);

		if (out) {
			out->type = ACPI_RESOURCE_TYPE_IO;
			out->data.io.io_decode = in->io.decode_type;
			out->data.io.alignment = in->io.alignment;
			out->data.io.address_length = in->io.length;
			out->data.io.minimum = in->io.minimum;
			out->data.io.maximum = in->io.maximum;
		}
		break;
	case UACPI_RESOURCE_TYPE_FIXED_IO:
		*length += sizeof(struct acpi_resource_fixed_io);

		if (out) {
			out->type = ACPI_RESOURCE_TYPE_IO;
			out->data.fixed_io.address = in->fixed_io.address;
			out->data.fixed_io.address_length = in->fixed_io.length;
		}
		break;
	case UACPI_RESOURCE_TYPE_ADDRESS16:
		*length += sizeof(struct acpi_resource_address16) +
			   uacpi_resource_source_size(&in->address16.source);

		if (out) {
			out->type = ACPI_RESOURCE_TYPE_ADDRESS16;

			acpi_st = uacpi_to_acpi_address_common(
				&in->address16.common, &out->data.address);
			if (acpi_st != AE_OK)
				return acpi_st;

			out->data.address16.address.granularity =
				in->address16.granularity;
			out->data.address16.address.minimum =
				in->address16.minimum;
			out->data.address16.address.maximum =
				in->address16.maximum;
			out->data.address16.address.translation_offset =
				in->address16.translation_offset;
			out->data.address16.address.address_length =
				in->address16.address_length;
			uacpi_to_acpi_resource_source(
				&in->address16.source,
				&out->data.address16.resource_source,
				&out->data.address16 + 1);
		}
		break;
	case UACPI_RESOURCE_TYPE_ADDRESS32:
		*length += sizeof(struct acpi_resource_address32) +
			   uacpi_resource_source_size(&in->address32.source);

		if (out) {
			out->type = ACPI_RESOURCE_TYPE_ADDRESS32;

			acpi_st = uacpi_to_acpi_address_common(
				&in->address32.common, &out->data.address);
			if (acpi_st != AE_OK)
				return acpi_st;

			out->data.address32.address.granularity =
				in->address32.granularity;
			out->data.address32.address.minimum =
				in->address32.minimum;
			out->data.address32.address.maximum =
				in->address32.maximum;
			out->data.address32.address.translation_offset =
				in->address32.translation_offset;
			out->data.address32.address.address_length =
				in->address32.address_length;
			uacpi_to_acpi_resource_source(
				&in->address32.source,
				&out->data.address32.resource_source,
				&out->data.address32 + 1);
		}
		break;
	case UACPI_RESOURCE_TYPE_ADDRESS64:
		*length += sizeof(struct acpi_resource_address64) +
			   uacpi_resource_source_size(&in->address64.source);

		if (out) {
			out->type = ACPI_RESOURCE_TYPE_ADDRESS64;

			acpi_st = uacpi_to_acpi_address_common(
				&in->address64.common, &out->data.address);
			if (acpi_st != AE_OK)
				return acpi_st;

			out->data.address64.address.granularity =
				in->address64.granularity;
			out->data.address64.address.minimum =
				in->address64.minimum;
			out->data.address64.address.maximum =
				in->address64.maximum;
			out->data.address64.address.translation_offset =
				in->address64.translation_offset;
			out->data.address64.address.address_length =
				in->address64.address_length;
			uacpi_to_acpi_resource_source(
				&in->address64.source,
				&out->data.address64.resource_source,
				&out->data.address64 + 1);
		}
		break;
	case UACPI_RESOURCE_TYPE_FIXED_MEMORY32:
		*length += sizeof(struct acpi_resource_fixed_memory32);

		if (out) {
			out->type = ACPI_RESOURCE_TYPE_FIXED_MEMORY32;
			out->data.fixed_memory32.write_protect =
				in->fixed_memory32.write_status;
			out->data.fixed_memory32.address =
				in->fixed_memory32.address;
			out->data.fixed_memory32.address_length =
				in->fixed_memory32.length;
		}
		break;
	case UACPI_RESOURCE_TYPE_END_TAG:
		*length += sizeof(struct acpi_resource_end_tag);

		if (out) {
			out->type = ACPI_RESOURCE_TYPE_END_TAG;
			DO_ONCE(acpi_os_printf,
				"ACPI: WARN: uacpi_to_acpi_resource does not support checksums\n");
			out->data.end_tag.checksum = 0;
		}
		break;
	default:
		acpi_os_printf(
			"ACPI: WARN: uacpi_to_acpi_resource does not support %u\n",
			in->type);
		*length = 0;
		return AE_SUPPORT;
	}

	*length = ALIGN(*length, sizeof(acpi_size));
	if (out)
		out->length = *length;
	return AE_OK;
}

acpi_status acpi_get_current_resources(acpi_handle device_handle,
				       struct acpi_buffer *ret_buffer)
{
	BUG();
}

acpi_status acpi_get_possible_resources(acpi_handle device_handle,
					struct acpi_buffer *ret_buffer)
{
	BUG();
}

acpi_status acpi_set_current_resources(acpi_handle device_handle,
				       struct acpi_buffer *in_buffer)
{
	BUG();
}

acpi_status acpi_get_event_resources(acpi_handle device_handle,
				     struct acpi_buffer *ret_buffer)
{
	BUG();
}

acpi_status acpi_get_irq_routing_table(acpi_handle device_handle,
				       struct acpi_buffer *ret_buffer)
{
	BUG();
}

acpi_status acpi_get_vendor_resource(acpi_handle device_handle, char *name,
				     struct acpi_vendor_uuid *uuid,
				     struct acpi_buffer *ret_buffer)
{
	BUG();
}

acpi_status acpi_buffer_to_resource(u8 *aml_buffer, u16 aml_buffer_length,
				    struct acpi_resource **resource_ptr)
{
	BUG();
}

acpi_status acpi_resource_to_address64(struct acpi_resource *resource,
				       struct acpi_resource_address64 *out)
{
	if (!resource || !out)
		return AE_BAD_PARAMETER;

	switch (resource->type) {
	case ACPI_RESOURCE_TYPE_ADDRESS16:
		out->address.granularity =
			resource->data.address16.address.granularity;
		out->address.minimum = resource->data.address16.address.minimum;
		out->address.maximum = resource->data.address16.address.maximum;
		out->address.translation_offset =
			resource->data.address16.address.translation_offset;
		out->address.address_length =
			resource->data.address16.address.address_length;
		break;
	case ACPI_RESOURCE_TYPE_ADDRESS32:
		out->address.granularity =
			resource->data.address32.address.granularity;
		out->address.minimum = resource->data.address32.address.minimum;
		out->address.maximum = resource->data.address32.address.maximum;
		out->address.translation_offset =
			resource->data.address32.address.translation_offset;
		out->address.address_length =
			resource->data.address32.address.address_length;
		break;
	case ACPI_RESOURCE_TYPE_ADDRESS64:
		out->address.granularity =
			resource->data.address64.address.granularity;
		out->address.minimum = resource->data.address64.address.minimum;
		out->address.maximum = resource->data.address64.address.maximum;
		out->address.translation_offset =
			resource->data.address64.address.translation_offset;
		out->address.address_length =
			resource->data.address64.address.address_length;
		break;
	default:
		return AE_BAD_PARAMETER;
	}

	out->resource_type = resource->data.address.resource_type;
	out->producer_consumer = resource->data.address.producer_consumer;
	out->decode = resource->data.address.decode;
	out->min_address_fixed = resource->data.address.min_address_fixed;
	out->max_address_fixed = resource->data.address.max_address_fixed;
	out->info = resource->data.address.info;

	return AE_OK;
}

struct shim_context {
	acpi_walk_resource_callback callback;
	void *context;

	acpi_status status;
};

static uacpi_iteration_decision shim_walk_resource(void *context,
						   uacpi_resource *resource)
{
	struct shim_context *c;
	struct acpi_resource local_acpi_resource;
	struct acpi_resource *acpi_resource_ptr;
	u32 length;
	bool allocated;
	acpi_status st;

	c = context;
	if (c->status != AE_OK)
		return UACPI_ITERATION_DECISION_BREAK;

	st = uacpi_to_acpi_resource(resource, NULL, &length);
	if (st != AE_OK)
		return UACPI_ITERATION_DECISION_CONTINUE;

	if (length <= sizeof(local_acpi_resource)) {
		allocated = false;
		acpi_resource_ptr = &local_acpi_resource;
	} else {
		allocated = true;
		acpi_resource_ptr = uacpi_kernel_alloc(length);
		if (!acpi_resource_ptr)
			return UACPI_ITERATION_DECISION_CONTINUE;
	}

	st = uacpi_to_acpi_resource(resource, acpi_resource_ptr, &length);
	if (st != AE_OK) {
		if (allocated)
			uacpi_kernel_free(acpi_resource_ptr);
		return UACPI_ITERATION_DECISION_CONTINUE;
	}

	st = c->callback(acpi_resource_ptr, c->context);
	if (allocated)
		uacpi_kernel_free(acpi_resource_ptr);

	switch (st) {
	case AE_OK:
		return UACPI_ITERATION_DECISION_CONTINUE;
	case AE_CTRL_TERMINATE:
		return UACPI_ITERATION_DECISION_BREAK;
	case AE_CTRL_DEPTH:
		return UACPI_ITERATION_DECISION_NEXT_PEER;
	default:
		c->status = st;
		return UACPI_ITERATION_DECISION_BREAK;
	}
}

acpi_status acpi_walk_resource_buffer(struct acpi_buffer *buffer,
				      acpi_walk_resource_callback user_function,
				      void *context)
{
	struct acpi_resource *resource;
	acpi_status acpi_st;

	if (!buffer || !buffer->pointer || !user_function)
		return AE_BAD_PARAMETER;

	resource = buffer->pointer;
	while (resource->type != ACPI_RESOURCE_TYPE_END_TAG) {
		acpi_st = user_function(resource, context);
		if (acpi_st == AE_CTRL_TERMINATE)
			return AE_OK;

		WARN_ON_ONCE(acpi_st == AE_CTRL_DEPTH);
		if (acpi_st != AE_OK)
			return acpi_st;

		resource = ACPI_NEXT_RESOURCE(resource);
	}

	return AE_OK;
}

acpi_status acpi_walk_resources(acpi_handle device_handle, char *name,
				acpi_walk_resource_callback user_function,
				void *context)
{
	uacpi_namespace_node *node;
	struct shim_context shim_context;
	uacpi_status st;

	if (!user_function)
		return AE_BAD_PARAMETER;

	if (strcmp(name, "_CRS") != 0 && strcmp(name, "_PRS") != 0 &&
	    strcmp(name, "_AEI") != 0)
		return AE_BAD_PARAMETER;

	shim_context.callback = user_function;
	shim_context.context = context;
	shim_context.status = AE_OK;

	node = uacpi_node_from_handle(device_handle);
	st = uacpi_for_each_device_resource(node, name, shim_walk_resource,
					    &shim_context);

	if (shim_context.status != AE_OK)
		return shim_context.status;
	return uacpi_convert_status(st);
}
