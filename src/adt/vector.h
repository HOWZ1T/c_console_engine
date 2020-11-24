#ifndef ADT_VECTOR_H
#define ADT_VECTOR_H

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct vec {
    void **arr;
    int capacity, size;
} vec;

vec vec_create(int capacity);
size_t vec_memory(vec* v);

void vec_add(vec* v, void *element);
void *vec_remove(vec* v);
void *vec_at(vec* v, int i);
void vec_dump(char* buffer, vec* v);
void vec_free(vec* v);

#endif //ADT_VECTOR_H
