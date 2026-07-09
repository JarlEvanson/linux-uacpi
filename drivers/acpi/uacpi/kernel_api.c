#include <linux/acpi.h>
#include <linux/interrupt.h>
#include <linux/sched/clock.h>

#include <uacpi/kernel_api.h>

#include "shim_shared.h"

uacpi_status __ref uacpi_kernel_get_rsdp(uacpi_phys_addr *out_rsdp_address)
{
	acpi_physical_address rsdp_address;

	rsdp_address = acpi_os_get_root_pointer();
	if (!rsdp_address)
		return UACPI_STATUS_NOT_FOUND;

	*out_rsdp_address = rsdp_address;
	return UACPI_STATUS_OK;
}

void *uacpi_kernel_map(uacpi_phys_addr addr, uacpi_size len)
{
	void *virt;

	virt = acpi_os_map_memory(addr, len);
	if (virt == NULL) {
		return UACPI_MAP_FAILED;
	}

	return virt;
}

void uacpi_kernel_unmap(void *addr, uacpi_size len)
{
	acpi_os_unmap_memory(addr, len);
}

void uacpi_kernel_log(uacpi_log_level level, const uacpi_char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);

	acpi_os_vprintf(fmt, args);

	va_end(args);
}

uacpi_status uacpi_kernel_pci_device_open(uacpi_pci_address address,
					  uacpi_handle *out_handle)
{
#ifdef CONFIG_PCI
	struct acpi_pci_id *dev;

	dev = acpi_os_allocate(sizeof(*dev));
	if (!dev)
		return UACPI_STATUS_OUT_OF_MEMORY;

	dev->segment = address.segment;
	dev->bus = address.bus;
	dev->device = address.device;
	dev->function = address.function;

	*out_handle = (uacpi_handle)dev;
	return UACPI_STATUS_OK;
#else
	return UACPI_STATUS_NOT_FOUND;
#endif
}

void uacpi_kernel_pci_device_close(uacpi_handle handle)
{
#ifdef CONFIG_PCI
	acpi_os_free(handle);
#endif
}

uacpi_status uacpi_kernel_pci_read8(uacpi_handle device, uacpi_size offset,
				    uacpi_u8 *value)
{
#ifdef CONFIG_PCI
	struct acpi_pci_id *dev = device;
	u64 val64;
	acpi_status st;

	st = acpi_os_read_pci_configuration(dev, offset, &val64, 8);
	*value = (uacpi_u8)val64;
	return uacpi_convert_status(st);
#else
	return UACPI_STATUS_UNIMPLEMENTED;
#endif
}

uacpi_status uacpi_kernel_pci_read16(uacpi_handle device, uacpi_size offset,
				     uacpi_u16 *value)
{
#ifdef CONFIG_PCI
	struct acpi_pci_id *dev = device;
	u64 val64;
	acpi_status st;

	st = acpi_os_read_pci_configuration(dev, offset, &val64, 16);
	*value = (uacpi_u16)val64;
	return uacpi_convert_status(st);
#else
	return UACPI_STATUS_UNIMPLEMENTED;
#endif
}

uacpi_status uacpi_kernel_pci_read32(uacpi_handle device, uacpi_size offset,
				     uacpi_u32 *value)
{
#ifdef CONFIG_PCI
	struct acpi_pci_id *dev = device;
	u64 val64;
	acpi_status st;

	st = acpi_os_read_pci_configuration(dev, offset, &val64, 32);
	*value = (uacpi_u32)val64;
	return uacpi_convert_status(st);
#else
	return UACPI_STATUS_UNIMPLEMENTED;
#endif
}

uacpi_status uacpi_kernel_pci_write8(uacpi_handle device, uacpi_size offset,
				     uacpi_u8 value)
{
#ifdef CONFIG_PCI
	struct acpi_pci_id *dev = device;
	acpi_status st;

	st = acpi_os_write_pci_configuration(dev, offset, value, 8);
	return uacpi_convert_status(st);
#else
	return UACPI_STATUS_UNIMPLEMENTED;
#endif
}

uacpi_status uacpi_kernel_pci_write16(uacpi_handle device, uacpi_size offset,
				      uacpi_u16 value)
{
#ifdef CONFIG_PCI
	struct acpi_pci_id *dev = device;
	acpi_status st;

	st = acpi_os_write_pci_configuration(dev, offset, value, 16);
	return uacpi_convert_status(st);
#else
	return UACPI_STATUS_UNIMPLEMENTED;
#endif
}

uacpi_status uacpi_kernel_pci_write32(uacpi_handle device, uacpi_size offset,
				      uacpi_u32 value)
{
#ifdef CONFIG_PCI
	struct acpi_pci_id *dev = device;
	acpi_status st;

	st = acpi_os_write_pci_configuration(dev, offset, value, 32);
	return uacpi_convert_status(st);
#else
	return UACPI_STATUS_UNIMPLEMENTED;
#endif
}

uacpi_status uacpi_kernel_io_map(uacpi_io_addr base, uacpi_size len,
				 uacpi_handle *out_handle)
{
	*out_handle = (uacpi_handle)(uintptr_t)base;
	return UACPI_STATUS_OK;
}

void uacpi_kernel_io_unmap(uacpi_handle handle)
{
	// No-op for this representation.
}

uacpi_status uacpi_kernel_io_read8(uacpi_handle handle, uacpi_size offset,
				   uacpi_u8 *out_value)
{
	u32 val;
	acpi_status st;
	acpi_io_address addr = (acpi_io_address)(uintptr_t)handle + offset;

	st = acpi_os_read_port(addr, &val, 8);
	*out_value = (uacpi_u8)val;
	return uacpi_convert_status(st);
}

uacpi_status uacpi_kernel_io_read16(uacpi_handle handle, uacpi_size offset,
				    uacpi_u16 *out_value)
{
	u32 val;
	acpi_status st;
	acpi_io_address addr = (acpi_io_address)(uintptr_t)handle + offset;

	st = acpi_os_read_port(addr, &val, 16);
	*out_value = (uacpi_u16)val;
	return uacpi_convert_status(st);
}

uacpi_status uacpi_kernel_io_read32(uacpi_handle handle, uacpi_size offset,
				    uacpi_u32 *out_value)
{
	u32 val;
	acpi_status st;
	acpi_io_address addr = (acpi_io_address)(uintptr_t)handle + offset;

	st = acpi_os_read_port(addr, &val, 32);
	*out_value = (uacpi_u32)val;
	return uacpi_convert_status(st);
}

uacpi_status uacpi_kernel_io_write8(uacpi_handle handle, uacpi_size offset,
				    uacpi_u8 in_value)
{
	acpi_io_address addr = (acpi_io_address)(uintptr_t)handle + offset;
	acpi_status st = acpi_os_write_port(addr, in_value, 8);

	return uacpi_convert_status(st);
}

uacpi_status uacpi_kernel_io_write16(uacpi_handle handle, uacpi_size offset,
				     uacpi_u16 in_value)
{
	acpi_io_address addr = (acpi_io_address)(uintptr_t)handle + offset;
	acpi_status st = acpi_os_write_port(addr, in_value, 16);

	return uacpi_convert_status(st);
}

uacpi_status uacpi_kernel_io_write32(uacpi_handle handle, uacpi_size offset,
				     uacpi_u32 in_value)
{
	acpi_io_address addr = (acpi_io_address)(uintptr_t)handle + offset;
	acpi_status st = acpi_os_write_port(addr, in_value, 32);

	return uacpi_convert_status(st);
}

uacpi_u8 uacpi_kernel_mmio_read8(void *ptr)
{
	return readb((void __iomem *)ptr);
}

uacpi_u16 uacpi_kernel_mmio_read16(void *ptr)
{
	return readw((void __iomem *)ptr);
}

uacpi_u32 uacpi_kernel_mmio_read32(void *ptr)
{
	return readl((void __iomem *)ptr);
}

uacpi_u64 uacpi_kernel_mmio_read64(void *ptr)
{
	return readq((void __iomem *)ptr);
}

void uacpi_kernel_mmio_write8(void *ptr, uacpi_u8 value)
{
	writeb(value, (void __iomem *)ptr);
}

void uacpi_kernel_mmio_write16(void *ptr, uacpi_u16 value)
{
	writew(value, (void __iomem *)ptr);
}

void uacpi_kernel_mmio_write32(void *ptr, uacpi_u32 value)
{
	writel(value, (void __iomem *)ptr);
}

void uacpi_kernel_mmio_write64(void *ptr, uacpi_u64 value)
{
	writeq(value, (void __iomem *)ptr);
}

void *uacpi_kernel_alloc(uacpi_size size)
{
	return acpi_os_allocate(size);
}

void uacpi_kernel_free(void *mem)
{
	acpi_os_free(mem);
}

uacpi_u64 uacpi_kernel_get_nanoseconds_since_boot(void)
{
	/*
	 * During testing, the use of `acpi_os_get_timer()` resulting in uACPI
	 * reporting a poor time source precision.
	 */
	return (uacpi_u64)sched_clock();
}

void uacpi_kernel_stall(uacpi_u8 usec)
{
	acpi_os_stall(usec);
}

void uacpi_kernel_sleep(uacpi_u64 msec)
{
	acpi_os_sleep(msec);
}

uacpi_handle uacpi_kernel_create_mutex(void)
{
	acpi_mutex mutex;
	acpi_status st;

	st = acpi_os_create_mutex(&mutex);
	if (st != AE_OK)
		return UACPI_NULL;

	return (uacpi_handle)mutex;
}

void uacpi_kernel_free_mutex(uacpi_handle handle)
{
	acpi_os_delete_mutex((acpi_mutex)handle);
}

uacpi_handle uacpi_kernel_create_event(void)
{
	acpi_semaphore semaphore;
	acpi_status st;

	st = acpi_os_create_semaphore(U32_MAX, 0, &semaphore);
	if (st != AE_OK)
		return UACPI_NULL;

	return (uacpi_handle)semaphore;
}

void uacpi_kernel_free_event(uacpi_handle handle)
{
	acpi_status st;

	st = acpi_os_delete_semaphore((acpi_semaphore)handle);
	if (st != AE_OK)
		acpi_os_printf("ACPI: error freeing uACPI event\n");
}

uacpi_thread_id uacpi_kernel_get_thread_id(void)
{
	return (uacpi_thread_id)acpi_os_get_thread_id();
}

uacpi_interrupt_state uacpi_kernel_disable_interrupts(void)
{
	unsigned long flags;
	local_irq_save(flags);
	return (uacpi_interrupt_state)flags;
}

void uacpi_kernel_restore_interrupts(uacpi_interrupt_state state)
{
	unsigned long flags = (unsigned long)state;
	local_irq_restore(flags);
}

uacpi_status uacpi_kernel_acquire_mutex(uacpi_handle handle, uacpi_u16 timeout)
{
	acpi_status st;

	st = acpi_os_acquire_mutex((acpi_mutex)handle, timeout);
	return uacpi_convert_status(st);
}

void uacpi_kernel_release_mutex(uacpi_handle handle)
{
	acpi_os_release_mutex((acpi_mutex)handle);
}

uacpi_bool uacpi_kernel_wait_for_event(uacpi_handle handle, uacpi_u16 timeout)
{
	acpi_status st;

	st = acpi_os_wait_semaphore((acpi_semaphore)handle, 1, timeout);
	if (st != AE_OK)
		return UACPI_FALSE;

	return UACPI_TRUE;
}

void uacpi_kernel_signal_event(uacpi_handle handle)
{
	acpi_status st;

	st = acpi_os_signal_semaphore((acpi_semaphore)handle, 1);
	if (st != AE_OK)
		acpi_os_printf("ACPI: error signaling uACPI event\n");
}

void uacpi_kernel_reset_event(uacpi_handle handle)
{
	while (uacpi_kernel_wait_for_event(handle, 0))
		;
}

uacpi_status
uacpi_kernel_handle_firmware_request(uacpi_firmware_request *request)
{
	struct acpi_signal_fatal_info fatal_info;
	acpi_status st;

	switch (request->type) {
	case UACPI_FIRMWARE_REQUEST_TYPE_BREAKPOINT:
		st = acpi_os_signal(ACPI_SIGNAL_BREAKPOINT, "not implemented");
		break;
	case UACPI_FIRMWARE_REQUEST_TYPE_FATAL:
		fatal_info.type = request->fatal.type;
		fatal_info.code = request->fatal.code;
		fatal_info.argument = request->fatal.arg;

		st = acpi_os_signal(ACPI_SIGNAL_FATAL, &fatal_info);
		break;
	default:
		pr_err("ACPI: unknown firmware request\n");
		return UACPI_STATUS_INVALID_ARGUMENT;
	}

	return uacpi_convert_status(st);
}

static LIST_HEAD(uacpi_irqs_list);
static DEFINE_SPINLOCK(uacpi_irqs_lock);

struct uacpi_irq_context {
	u32 irq;
	uacpi_interrupt_handler handler;
	uacpi_handle ctx;
	struct list_head node;
};

static u32 uacpi_irq_stub(void *context)
{
	struct uacpi_irq_context *irq_ctx = context;
	return (u32)irq_ctx->handler(irq_ctx->ctx);
}

uacpi_status uacpi_kernel_install_interrupt_handler(
	uacpi_u32 irq, uacpi_interrupt_handler handler, uacpi_handle ctx,
	uacpi_handle *out_irq_handle)
{
	struct uacpi_irq_context *irq_ctx;
	acpi_status st;

	irq_ctx = acpi_os_allocate(sizeof(*irq_ctx));
	if (!irq_ctx)
		return UACPI_STATUS_OUT_OF_MEMORY;

	irq_ctx->irq = irq;
	irq_ctx->handler = handler;
	irq_ctx->ctx = ctx;
	INIT_LIST_HEAD(&irq_ctx->node);

	st = acpi_os_install_interrupt_handler(irq, uacpi_irq_stub, irq_ctx);
	if (st != AE_OK) {
		acpi_os_free(irq_ctx);
		return uacpi_convert_status(st);
	}

	spin_lock_irq(&uacpi_irqs_lock);
	list_add_tail(&irq_ctx->node, &uacpi_irqs_list);
	spin_unlock_irq(&uacpi_irqs_lock);

	*out_irq_handle = (uacpi_handle)irq_ctx;
	return UACPI_STATUS_OK;
}

uacpi_status
uacpi_kernel_uninstall_interrupt_handler(uacpi_interrupt_handler handler,
					 uacpi_handle irq_handle)
{
	struct uacpi_irq_context *irq_ctx = irq_handle;
	acpi_status st;

	if (!irq_ctx)
		return UACPI_STATUS_INVALID_ARGUMENT;

	spin_lock_irq(&uacpi_irqs_lock);
	list_del(&irq_ctx->node);
	spin_unlock_irq(&uacpi_irqs_lock);

	st = acpi_os_remove_interrupt_handler(irq_ctx->irq, uacpi_irq_stub);
	acpi_os_free(irq_ctx);

	return uacpi_convert_status(st);
}

uacpi_handle uacpi_kernel_create_spinlock(void)
{
	acpi_spinlock spinlock;
	acpi_status st;

	st = acpi_os_create_lock(&spinlock);
	if (st != AE_OK)
		return UACPI_NULL;

	return (uacpi_handle)spinlock;
}

void uacpi_kernel_free_spinlock(uacpi_handle handle)
{
	acpi_os_delete_lock((acpi_spinlock)handle);
}

uacpi_cpu_flags uacpi_kernel_lock_spinlock(uacpi_handle handle)
{
	return (uacpi_cpu_flags)acpi_os_acquire_lock((acpi_spinlock)handle);
}

void uacpi_kernel_unlock_spinlock(uacpi_handle handle, uacpi_cpu_flags flags)
{
	acpi_os_release_lock((acpi_spinlock)handle, (acpi_cpu_flags)flags);
}

static struct workqueue_struct *uacpi_wq;

struct uacpi_work_ctx {
	struct work_struct work;
	uacpi_work_handler handler;
	uacpi_handle ctx;
};

static void uacpi_work_stub_handler(struct work_struct *work)
{
	struct uacpi_work_ctx *work_ctx =
		container_of(work, struct uacpi_work_ctx, work);

	work_ctx->handler(work_ctx->ctx);
	kfree(work_ctx);
}

uacpi_status uacpi_kernel_schedule_work(uacpi_work_type type,
					uacpi_work_handler handler,
					uacpi_handle ctx)
{
	struct uacpi_work_ctx *work_ctx;

	work_ctx = acpi_os_allocate(sizeof(*work_ctx));
	if (!work_ctx)
		return UACPI_STATUS_OUT_OF_MEMORY;

	INIT_WORK(&work_ctx->work, uacpi_work_stub_handler);
	work_ctx->handler = handler;
	work_ctx->ctx = ctx;

	if (type == UACPI_WORK_GPE_EXECUTION) {
		queue_work_on(0, uacpi_wq, &work_ctx->work);
	} else {
		queue_work(uacpi_wq, &work_ctx->work);
	}

	return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_wait_for_work_completion(void)
{
	struct uacpi_irq_context *irq_ctx;

	spin_lock_irq(&uacpi_irqs_lock);
	list_for_each_entry(irq_ctx, &uacpi_irqs_list, node) {
		synchronize_irq(irq_ctx->irq);
	}
	spin_unlock_irq(&uacpi_irqs_lock);

	if (uacpi_wq)
		flush_workqueue(uacpi_wq);

	return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_initialize(uacpi_init_level current_init_lvl)
{
	uacpi_status st;

	if (current_init_lvl == UACPI_INIT_LEVEL_EARLY) {
		uacpi_wq = alloc_workqueue("uacpi_wq",
					   WQ_UNBOUND | WQ_MEM_RECLAIM, 0);
		if (!uacpi_wq)
			return UACPI_STATUS_OUT_OF_MEMORY;

		shim_tables_initialize();
		st = shim_namespace_initialize();
		if (st != UACPI_STATUS_OK)
			return st;
	}

	return UACPI_STATUS_OK;
}

void uacpi_kernel_deinitialize(void)
{
	if (uacpi_wq) {
		flush_workqueue(uacpi_wq);
		destroy_workqueue(uacpi_wq);
		uacpi_wq = NULL;
	}

	shim_tables_deinitialize();
	shim_namespace_deinitialize();
}
