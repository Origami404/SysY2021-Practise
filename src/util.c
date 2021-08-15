#include "util.h"

string String(char const* s) {
    char *p = malloc(strlen(s) + 1);
    strcpy(p, s);
    return p;
}

void _debug(char const *file, int line, char const *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);

    fprintf(stderr, "Debug at %s:%d: ", file, line);
    vfprintf(stderr, fmt, argp);
    fprintf(stderr, "\n");

    va_end(argp);
}

void _panic(char const *file, int line, char const *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);

    fprintf(stderr, "Panic at %s:%d: ", file, line);
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

void* checked_realloc(void *data, size_t s) {
    void *p = realloc(data, s);
    if (!p) panic("Memory error");
    return p;
}