# SPDX-License-Identifier: GPL-2.0
#
# Makefile for uACPI compatibility layer
#

ccflags-y := -I $(srctree)/drivers/acpi/uacpi/lib/include
ccflags-y += -I $(srctree)/drivers/acpi/uacpi

# Header overrides
ccflags-y += -DUACPI_OVERRIDE_ARCH_HELPERS
ccflags-y += -DUACPI_OVERRIDE_LIBC
ccflags-y += -DUACPI_OVERRIDE_TYPES

# uACPI config options
ccflags-y += -DUACPI_FORMATTED_LOGGING
ccflags-y += -DUACPI_KERNEL_INITIALIZATION
ccflags-y += -DUACPI_NATIVE_MMIO
ccflags-y += -DUACPI_START_OF_LOG_MSG='"ACPI: "'

ccflags-$(ACPI_REDUCED_HARDWARE_ONLY) += -DUACPI_REDUCED_HARDWARE

obj-y += acpi.o

acpi-y += kernel_api.o

acpi-y += shim_fixed_event.o
acpi-y += shim_globals.o
acpi-y += shim_gpe_event.o
acpi-y += shim_handler.o
acpi-y += shim_hardware.o
acpi-y += shim_interface.o
acpi-y += shim_misc.o
acpi-y += shim_namespace.o
acpi-y += shim_object.o
acpi-y += shim_op_region.o
acpi-y += shim_output.o
acpi-y += shim_resource.o
acpi-y += shim_sleep.o
acpi-y += shim_subsystem.o
acpi-y += shim_tables.o

uacpi-files :=			\
	default_handlers.o	\
	event.o			\
	interpreter.o		\
	io.o			\
	mutex.o			\
	namespace.o		\
	notify.o		\
	opcodes.o		\
	opregion.o		\
	osi.o			\
	registers.o		\
	resources.o		\
	shareable.o		\
	sleep.o			\
	stdlib.o		\
	tables.o		\
	types.o			\
	uacpi.o			\
	utilities.o

uacpi-files := $(addprefix lib/source/, $(uacpi-files))
acpi-y += $(uacpi-files)

# Suppress various warnings for the external library files
$(foreach obj, $(uacpi-files), $(eval CFLAGS_$(obj) += \
	-Wno-unused-parameter \
	-Wno-unused-but-set-variable \
	-Wno-sign-compare \
	-Wno-missing-prototypes \
	-Wno-missing-declarations \
	-Wno-override-init \
	-Wno-shadow \
	-Wno-format))
