#ifndef HEADER_UTIL_H__
#define HEADER_UTIL_H__

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

#endif // HEADER_UTIL_H__