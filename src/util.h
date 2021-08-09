#ifndef HEADER_UTIL_H__
#define HEADER_UTIL_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

typedef char *string;

string String(char const* s) {
    char *p = malloc(strlen(s) + 1);
    strcpy(p, s);
    return p;
}

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define panic(...) _panic(__FILE__, __LINE__, __VA_ARGS__)
void _panic(char const *file, int line, char const *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);

    fprintf(stderr, "Panic at %s %d: ", file, line);
    vfprintf(stderr, fmt, argp);
    fprintf(stderr, "\n");

    va_end(argp);
    exit(-1);
}

void* checked_malloc(size_t s) {
    void *p = malloc(s);
    if (!p) panic("Memory error");
    return p;
}

void checked_realloc(void *data, size_t s) {
    void *p = realloc(data, s);
    if (!p) panic("Memory error");
}

#endif // HEADER_UTIL_H__