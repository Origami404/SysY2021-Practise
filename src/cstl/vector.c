#include "util.h"

typedef struct vector {
    void *data;
    size_t length;
    size_t size;

    size_t rlen;
} *vector ;

#define VECTOR_DEF 1
#include "cstl.h"

vector vec_create(data_spec spec) {
    vector v = checked_malloc(sizeof(*v));

    spec.padding = spec.padding ? spec.padding : spec.size;

    // FIX: mul overflow
    v->data = checked_malloc(spec.size * spec.length); 
    void *end = spec.data + (spec.padding * spec.length);

    void *dst = v->data;
    for (void *src = spec.data; src < end; src += spec.padding) {
        memcpy(dst, src, spec.size);
        dst += spec.size;
    }

    v->length = spec.length;
    v->size = spec.size;
    v->rlen = spec.length;

    return v;
}

void vec_ensure_fit(vector v, vector o) {
    if (v->size != o->size) {
        panic("Size unfit: %u %u", v->size, o->size);
    }
}

void vec_extend_to(vector v, size_t len) {
    if (v->rlen < len) {
        checked_realloc(v->data, len);
        v->rlen = len;
    }
}

void* vec_end(vector v) {
    return (char*)v->data + v->length;
}

void vec_concat_cpy(vector v, vector o) {
    vec_ensure_fit(v, o);

    v->length += o->length;
    vec_extend_to(v, v->length);

    memcpy(vec_end(v), o->data, o->length * o->size);
}

void vec_concat_mov(vector v, vector o) {
    vec_ensure_fit(v, o);

    v->length += o->length;
    vec_extend_to(v, v->length);

    memmove(vec_end(v), o->data, o->length * o->size);
}   

void vec_add(vector v, void *data) {
    if (v->rlen == v->length) {
        vec_extend_to(v, v->rlen * 2);
    }

    memcpy(vec_end(v), data, v->size);
    v->length += 1;
}

void vec_pop(vector v) {
    v->length -= 1;
}

void* vec_get(vector v, int idx) {
    const int len = (int)v->length;

    if (-idx > len || idx >= len) {
        panic("Out of index: %d(len: %d)", idx, len);
    }

    if (idx < 0)
        idx += len;

    return (char*)v->data + idx;
}

void vec_set(vector v, int idx, void *data) {
    void *p = vec_get(v, idx);
    memcpy(p, data, v->size);
}

size_t vec_len(vector v) {
    return v->length;
}

void vec_destory(vector v) {
    free(v->data);
    free(v);
}

