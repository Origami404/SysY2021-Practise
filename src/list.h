#ifndef HEADER_LIST_H__
#define HEADER_LIST_H__

#include "util.h"

#define DEF_LIST(name, type) \
    typedef struct name {    \
        type car;            \
        struct name *cdr;    \
    } *name;                 \
    name cons_##name(type car, name cdr) {   \
        name p = checked_malloc(sizeof(*p)); \
        p->car = car; p->cdr = cdr;          \
        return p;                            \
    } 


#endif // HEADER_LIST_H__