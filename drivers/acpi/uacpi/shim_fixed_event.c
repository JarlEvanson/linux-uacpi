#include <linux/acpi.h>

#include <uacpi/event.h>

#include "shim_shared.h"

static uacpi_status acpi_to_uacpi_event(u32 event,
					uacpi_fixed_event *uacpi_event)
{
	switch (event) {
	case ACPI_EVENT_PMTIMER:
		*uacpi_event = UACPI_FIXED_EVENT_TIMER_STATUS;
		break;
	case ACPI_EVENT_POWER_BUTTON:
		*uacpi_event = UACPI_FIXED_EVENT_POWER_BUTTON;
		break;
	case ACPI_EVENT_SLEEP_BUTTON:
		*uacpi_event = UACPI_FIXED_EVENT_SLEEP_BUTTON;
		break;
	case ACPI_EVENT_RTC:
		*uacpi_event = UACPI_FIXED_EVENT_RTC;
		break;
	default:
		acpi_os_printf("ACPI: unrecongized event: %x\n", event);
		return UACPI_STATUS_UNIMPLEMENTED;
	}

	return UACPI_STATUS_OK;
}

acpi_status acpi_enable_event(u32 event, u32 flags)
{
	uacpi_fixed_event fixed_event;
	uacpi_status st;

	st = acpi_to_uacpi_event(event, &fixed_event);
	if (st != UACPI_STATUS_OK)
		return AE_BAD_PARAMETER;

	st = uacpi_enable_fixed_event(fixed_event);
	return uacpi_convert_status(st);
}

acpi_status acpi_disable_event(u32 event, u32 flags)
{
	uacpi_fixed_event fixed_event;
	uacpi_status st;

	st = acpi_to_uacpi_event(event, &fixed_event);
	if (st != UACPI_STATUS_OK)
		return AE_BAD_PARAMETER;

	st = uacpi_disable_fixed_event(fixed_event);
	return uacpi_convert_status(st);
}

acpi_status acpi_clear_event(u32 event)
{
	uacpi_fixed_event fixed_event;
	uacpi_status st;

	st = acpi_to_uacpi_event(event, &fixed_event);
	if (st != UACPI_STATUS_OK)
		return AE_BAD_PARAMETER;

	st = uacpi_clear_fixed_event(fixed_event);
	return uacpi_convert_status(st);
}

acpi_status acpi_get_event_status(u32 event, acpi_event_status *event_status)
{
	uacpi_fixed_event fixed_event;
	uacpi_event_info out_info;
	uacpi_status st;

	if (!event_status)
		return AE_BAD_PARAMETER;

	st = acpi_to_uacpi_event(event, &fixed_event);
	if (st != UACPI_STATUS_OK)
		return AE_BAD_PARAMETER;

	st = uacpi_fixed_event_info(fixed_event, &out_info);
	if (st != UACPI_STATUS_OK)
		return uacpi_convert_status(st);

	*event_status = 0;

	if (out_info & UACPI_EVENT_INFO_HW_ENABLED)
		*event_status |= ACPI_EVENT_FLAG_ENABLED;

	if (out_info & UACPI_EVENT_INFO_HAS_HANDLER)
		*event_status |= ACPI_EVENT_FLAG_HAS_HANDLER;

	if (out_info & UACPI_EVENT_INFO_HW_STATUS)
		*event_status |= ACPI_EVENT_FLAG_SET;

	return AE_OK;
}

static const uacpi_fixed_event fixed_events[4] = {
	UACPI_FIXED_EVENT_TIMER_STATUS,
	UACPI_FIXED_EVENT_POWER_BUTTON,
	UACPI_FIXED_EVENT_SLEEP_BUTTON,
	UACPI_FIXED_EVENT_RTC,
};

u32 acpi_any_fixed_event_status_set(void)
{
	int i;
	uacpi_event_info info;
	uacpi_status st;

	for (i = 0; i < ARRAY_SIZE(fixed_events); i++) {
		st = uacpi_fixed_event_info(fixed_events[i], &info);
		if (st != UACPI_STATUS_OK)
			continue;

		if ((info & UACPI_EVENT_INFO_HW_ENABLED) &&
		    (info & UACPI_EVENT_INFO_HW_STATUS))
			return true;
	}

	return false;
}

struct shim_fixed_event_context {
	acpi_event_handler handler;
	void *context;
};

static DEFINE_MUTEX(shim_fixed_event_contexts_lock);
static struct shim_fixed_event_context
	shim_fixed_event_contexts[UACPI_FIXED_EVENT_MAX + 1];

static uacpi_interrupt_ret shim_fixed_event_handler(uacpi_handle handle)
{
	struct shim_fixed_event_context context;
	u32 result;

	mutex_lock(&shim_fixed_event_contexts_lock);
	context = *(struct shim_fixed_event_context *)handle;
	mutex_unlock(&shim_fixed_event_contexts_lock);

	result = context.handler(context.context);
	if (result == ACPI_INTERRUPT_HANDLED) {
		return UACPI_INTERRUPT_HANDLED;
	} else {
		return UACPI_INTERRUPT_NOT_HANDLED;
	}
}

acpi_status acpi_install_fixed_event_handler(u32 event,
					     acpi_event_handler handler,
					     void *context)
{
	uacpi_fixed_event fixed_event;
	uacpi_status st;

	if (!handler)
		return AE_BAD_PARAMETER;

	st = acpi_to_uacpi_event(event, &fixed_event);
	if (st != UACPI_STATUS_OK)
		return AE_BAD_PARAMETER;

	mutex_lock(&shim_fixed_event_contexts_lock);

	if (shim_fixed_event_contexts[fixed_event].handler)
		return AE_ALREADY_EXISTS;

	st = uacpi_install_fixed_event_handler(
		fixed_event, shim_fixed_event_handler, context);
	if (st != UACPI_STATUS_OK) {
		mutex_unlock(&shim_fixed_event_contexts_lock);
		return uacpi_convert_status(st);
	}

	shim_fixed_event_contexts[fixed_event].handler = handler;
	shim_fixed_event_contexts[fixed_event].context = context;

	mutex_unlock(&shim_fixed_event_contexts_lock);
	return AE_OK;
}

acpi_status acpi_remove_fixed_event_handler(u32 event,
					    acpi_event_handler handler)
{
	uacpi_fixed_event fixed_event;
	uacpi_status st;

	if (!handler)
		return AE_BAD_PARAMETER;

	st = acpi_to_uacpi_event(event, &fixed_event);
	if (st != UACPI_STATUS_OK)
		return AE_BAD_PARAMETER;

	mutex_lock(&shim_fixed_event_contexts_lock);

	if (!shim_fixed_event_contexts[fixed_event].handler) {
		mutex_unlock(&shim_fixed_event_contexts_lock);
		return AE_NOT_EXIST;
	}

	if (shim_fixed_event_contexts[fixed_event].handler != handler) {
		mutex_unlock(&shim_fixed_event_contexts_lock);
		return AE_BAD_PARAMETER;
	}

	st = uacpi_uninstall_fixed_event_handler(fixed_event);
	if (st != UACPI_STATUS_OK) {
		mutex_unlock(&shim_fixed_event_contexts_lock);
		return uacpi_convert_status(st);
	}

	shim_fixed_event_contexts[fixed_event].handler = NULL;
	shim_fixed_event_contexts[fixed_event].context = NULL;

	mutex_unlock(&shim_fixed_event_contexts_lock);
	return AE_OK;
}

void shim_fixed_event_initialize(void)
{
	int i;

	mutex_lock(&shim_fixed_event_contexts_lock);

	for (i = 0; i < ARRAY_SIZE(shim_fixed_event_contexts); i++) {
		shim_fixed_event_contexts[i].handler = NULL;
		shim_fixed_event_contexts[i].context = NULL;
	}

	mutex_unlock(&shim_fixed_event_contexts_lock);
}

void shim_fixed_event_deinitialize(void)
{
	int i;

	mutex_lock(&shim_fixed_event_contexts_lock);

	for (i = 0; i < ARRAY_SIZE(shim_fixed_event_contexts); i++) {
		shim_fixed_event_contexts[i].handler = NULL;
		shim_fixed_event_contexts[i].context = NULL;
	}

	mutex_unlock(&shim_fixed_event_contexts_lock);
}
