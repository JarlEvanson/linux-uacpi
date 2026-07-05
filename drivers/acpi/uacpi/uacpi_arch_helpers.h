#pragma once

#include <linux/compiler.h>

#define UACPI_ARCH_FLUSH_CPU_CACHE() \
	do {                         \
	} while (0)

typedef unsigned long uacpi_cpu_flags;
typedef unsigned long uacpi_interrupt_state;

typedef void *uacpi_thread_id;

#define UACPI_THREAD_ID_NONE ((uacpi_thread_id)UACPI_NULL)

#define UACPI_ATOMIC_LOAD_THREAD_ID(ptr) ((uacpi_thread_id)READ_ONCE(*(ptr)))

#define UACPI_ATOMIC_STORE_THREAD_ID(ptr, value) WRITE_ONCE(*(ptr), (value))
