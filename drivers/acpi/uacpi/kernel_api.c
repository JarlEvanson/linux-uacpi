#include <linux/bug.h>

#include <uacpi/kernel_api.h>

uacpi_status __ref uacpi_kernel_get_rsdp(uacpi_phys_addr *out_rsdp_address)
{
	BUG();
}

void *uacpi_kernel_map(uacpi_phys_addr addr, uacpi_size len)
{
	BUG();
}

void uacpi_kernel_unmap(void *addr, uacpi_size len)
{
	BUG();
}

void uacpi_kernel_log(uacpi_log_level level, const uacpi_char *fmt, ...)
{
	BUG();
}

uacpi_status uacpi_kernel_pci_device_open(uacpi_pci_address address,
					  uacpi_handle *out_handle)
{
	BUG();
}

void uacpi_kernel_pci_device_close(uacpi_handle handle)
{
	BUG();
}

uacpi_status uacpi_kernel_pci_read8(uacpi_handle device, uacpi_size offset,
				    uacpi_u8 *value)
{
	BUG();
}

uacpi_status uacpi_kernel_pci_read16(uacpi_handle device, uacpi_size offset,
				     uacpi_u16 *value)
{
	BUG();
}

uacpi_status uacpi_kernel_pci_read32(uacpi_handle device, uacpi_size offset,
				     uacpi_u32 *value)
{
	BUG();
}

uacpi_status uacpi_kernel_pci_write8(uacpi_handle device, uacpi_size offset,
				     uacpi_u8 value)
{
	BUG();
}

uacpi_status uacpi_kernel_pci_write16(uacpi_handle device, uacpi_size offset,
				      uacpi_u16 value)
{
	BUG();
}

uacpi_status uacpi_kernel_pci_write32(uacpi_handle device, uacpi_size offset,
				      uacpi_u32 value)
{
	BUG();
}

uacpi_status uacpi_kernel_io_map(uacpi_io_addr base, uacpi_size len,
				 uacpi_handle *out_handle)
{
	BUG();
}

void uacpi_kernel_io_unmap(uacpi_handle handle)
{
	BUG();
}

uacpi_status uacpi_kernel_io_read8(uacpi_handle handle, uacpi_size offset,
				   uacpi_u8 *out_value)
{
	BUG();
}

uacpi_status uacpi_kernel_io_read16(uacpi_handle handle, uacpi_size offset,
				    uacpi_u16 *out_value)
{
	BUG();
}

uacpi_status uacpi_kernel_io_read32(uacpi_handle handle, uacpi_size offset,
				    uacpi_u32 *out_value)
{
	BUG();
}

uacpi_status uacpi_kernel_io_write8(uacpi_handle handle, uacpi_size offset,
				    uacpi_u8 in_value)
{
	BUG();
}

uacpi_status uacpi_kernel_io_write16(uacpi_handle handle, uacpi_size offset,
				     uacpi_u16 in_value)
{
	BUG();
}

uacpi_status uacpi_kernel_io_write32(uacpi_handle handle, uacpi_size offset,
				     uacpi_u32 in_value)
{
	BUG();
}

uacpi_u8 uacpi_kernel_mmio_read8(void *ptr)
{
	BUG();
}

uacpi_u16 uacpi_kernel_mmio_read16(void *ptr)
{
	BUG();
}

uacpi_u32 uacpi_kernel_mmio_read32(void *ptr)
{
	BUG();
}

uacpi_u64 uacpi_kernel_mmio_read64(void *ptr)
{
	BUG();
}

void uacpi_kernel_mmio_write8(void *ptr, uacpi_u8 value)
{
	BUG();
}

void uacpi_kernel_mmio_write16(void *ptr, uacpi_u16 value)
{
	BUG();
}

void uacpi_kernel_mmio_write32(void *ptr, uacpi_u32 value)
{
	BUG();
}

void uacpi_kernel_mmio_write64(void *ptr, uacpi_u64 value)
{
	BUG();
}

void *uacpi_kernel_alloc(uacpi_size size)
{
	BUG();
}

void uacpi_kernel_free(void *mem)
{
	BUG();
}

uacpi_u64 uacpi_kernel_get_nanoseconds_since_boot(void)
{
	BUG();
}

void uacpi_kernel_stall(uacpi_u8 usec)
{
	BUG();
}

void uacpi_kernel_sleep(uacpi_u64 msec)
{
	BUG();
}

uacpi_handle uacpi_kernel_create_mutex(void)
{
	BUG();
}

void uacpi_kernel_free_mutex(uacpi_handle handle)
{
	BUG();
}

uacpi_handle uacpi_kernel_create_event(void)
{
	BUG();
}

void uacpi_kernel_free_event(uacpi_handle handle)
{
	BUG();
}

uacpi_thread_id uacpi_kernel_get_thread_id(void)
{
	BUG();
}

uacpi_interrupt_state uacpi_kernel_disable_interrupts(void)
{
	BUG();
}

void uacpi_kernel_restore_interrupts(uacpi_interrupt_state state)
{
	BUG();
}

uacpi_status uacpi_kernel_acquire_mutex(uacpi_handle handle, uacpi_u16 timeout)
{
	BUG();
}

void uacpi_kernel_release_mutex(uacpi_handle handle)
{
	BUG();
}

uacpi_bool uacpi_kernel_wait_for_event(uacpi_handle handle, uacpi_u16 timeout)
{
	BUG();
}

void uacpi_kernel_signal_event(uacpi_handle handle)
{
	BUG();
}

void uacpi_kernel_reset_event(uacpi_handle handle)
{
	BUG();
}

uacpi_status
uacpi_kernel_handle_firmware_request(uacpi_firmware_request *request)
{
	BUG();
}

uacpi_status uacpi_kernel_install_interrupt_handler(
	uacpi_u32 irq, uacpi_interrupt_handler handler, uacpi_handle ctx,
	uacpi_handle *out_irq_handle)
{
	BUG();
}

uacpi_status
uacpi_kernel_uninstall_interrupt_handler(uacpi_interrupt_handler handler,
					 uacpi_handle irq_handle)
{
	BUG();
}

uacpi_handle uacpi_kernel_create_spinlock(void)
{
	BUG();
}

void uacpi_kernel_free_spinlock(uacpi_handle handle)
{
	BUG();
}

uacpi_cpu_flags uacpi_kernel_lock_spinlock(uacpi_handle handle)
{
	BUG();
}

void uacpi_kernel_unlock_spinlock(uacpi_handle handle, uacpi_cpu_flags flags)
{
	BUG();
}

uacpi_status uacpi_kernel_schedule_work(uacpi_work_type type,
					uacpi_work_handler handler,
					uacpi_handle ctx)
{
	BUG();
}

uacpi_status uacpi_kernel_wait_for_work_completion(void)
{
	BUG();
}

uacpi_status uacpi_kernel_initialize(uacpi_init_level current_init_lvl)
{
	BUG();
}

void uacpi_kernel_deinitialize(void)
{
	BUG();
}
