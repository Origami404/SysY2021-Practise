#ifndef HEADER_UTIL_H__
#define HEADER_UTIL_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

typedef char *string;
string String(char const* s);

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define panic(...) _panic(__FILE__, __LINE__, __VA_ARGS__)
void _panic(char const *file, int line, char const *fmt, ...);

#ifdef DEBUG
#define debug(...) _debug(__FILE__, __LINE__, __VA_ARGS__) 
#else
#define debug(...) 
#endif

void _debug(char const *file, int line, char const *fmt, ...);

void* checked_malloc(size_t s);
void* checked_realloc(void *data, size_t s);

#endif // HEADER_UTIL_H__