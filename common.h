#pragma once
#include <stdarg.h>

#define va_list  __builtin_va_list
#define va_start __builtin_va_start
#define va_end   __builtin_va_end
#define va_arg   __builtin_va_arg

#define PRINTF_TYPE_INT 'd'
#define PRINTF_TYPE_STR 's'
#define PRINTF_TYPE_HEX 'x'

void printf(const char *fmt, ...);
