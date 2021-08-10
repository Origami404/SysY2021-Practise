#ifndef HEADER_LIST_H__
#define HEADER_LIST_H__

#define DEF_LIST(name, type) \
    typedef struct name {    \
        type car;            \
        struct name *cdr;    \
    } name;                  

#define cons(list_name, car_, cdr_) \
    (list_name) {.car = car_, .cdr = cdr_}

#endif // HEADER_LIST_H__