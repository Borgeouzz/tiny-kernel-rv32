#pragma once

// Memory page size of 4 KB
#define PAGE_SIZE 4096

// Define standard types
typedef int bool;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;
typedef uint32_t size_t;

typedef uint32_t paddr_t; // Type representing physical memory address
typedef uint32_t vaddr_t; // Type representing virtual memory address

// Boolean defines
#define true 1
#define false 0

// NULL define
#define NULL ((void *)0)

#define align_up(value, align) __builtin_align_up(value, align)     // return their first argument aligned up to the next multiple of the second argument
#define is_aligned(value, align) __builtin_is_aligned(value, align) // returns whether the first argument is aligned to a multiple of the second argument
#define offsetof(type, member) __builtin_offsetof(type, member)     // calculates the offset (in bytes) to a given member of the given type

// Variadic functions
#define va_list  __builtin_va_list
#define va_start __builtin_va_start
#define va_end   __builtin_va_end
#define va_arg   __builtin_va_arg

// Printf format chars
#define PRINTF_TYPE_INT 'd'
#define PRINTF_TYPE_STR 's'
#define PRINTF_TYPE_HEX 'x'

// Functions
void *memset(void *buf, char c, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
char *strcpy(char *dst, const char *src);
int strcmp(const char *s1, const char *s2);
void printf(const char *fmt, ...);
