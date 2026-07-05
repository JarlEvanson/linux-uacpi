#pragma once

#include <linux/types.h>
#include <linux/stdarg.h>

typedef u8 uacpi_u8;
typedef u16 uacpi_u16;
typedef u32 uacpi_u32;
typedef u64 uacpi_u64;

typedef s8 uacpi_i8;
typedef s16 uacpi_i16;
typedef s32 uacpi_i32;
typedef s64 uacpi_i64;

#define UACPI_TRUE true
#define UACPI_FALSE false
typedef bool uacpi_bool;

#define UACPI_NULL NULL

typedef unsigned long uacpi_uintptr;
typedef uacpi_uintptr uacpi_virt_addr;
typedef size_t uacpi_size;

typedef va_list uacpi_va_list;
#define uacpi_va_start va_start
#define uacpi_va_end va_end
#define uacpi_va_arg va_arg

typedef char uacpi_char;

#define uacpi_offsetof offsetof

#define UACPI_PRIu64 "llu"
#define UACPI_PRIx64 "llx"
#define UACPI_PRIX64 "llX"
#define UACPI_FMT64(val) ((unsigned long long)(val))
