#ifndef HEADER_CSTL_H__
#define HEADER_CSTL_H__

#include "util.h"

#ifndef VECTOR_DEF
typedef void *vector;
#endif // VECTOR_DEF

#define vector_t(type) vector

typedef struct data_spec {
    void *data;       // pointer to the data
    size_t size;      // the size of each element, in byte
    size_t length;    // how many element need to copy
    size_t padding;   // the padding between each element, in byte
} data_spec;

vector vec_create(data_spec spec);

void  vec_concat_cpy(vector v, vector other);
void  vec_concat_mov(vector v, vector other);
void  vec_add(vector v, void *data);
void  vec_pop(vector v);
void* vec_get(vector v, int idx);
void  vec_set(vector v, int idx, void *data);

void vec_destory(vector v);

#endif // HEADER_CSTL_H__