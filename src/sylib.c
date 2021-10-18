#ifdef SELF_DEFINED_SYLIB

#include <time.h>
#include "util.h"

int getint() {
    int x; 
    scanf("%d", &x); 
    return x;
}

int getch() {
    return getchar();
}

int getarray(int r[]) {
    int n; scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        scanf("%d", r + i);
    }
    return n;
}

void putint(int x) {
    printf("%d", x);
}

void putch(int x) {
    putchar(x);
}

void putarray(int n, int r[]) {
    printf("%d: ", n);
    for (int i = 0; i < (n-1); i++) {
        printf("%d ", r[i]);
    } 
    printf("%d", r[n-1]);
}

void putf(char const *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);

    vfprintf(stdin, fmt, argp);

    va_end(argp);
}

static int timer_cnt = 1;
static time_t start_time = 0;
static time_t total_time = 0;

#endif