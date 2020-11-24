#include "vector.h"

void vec_grow(vec* v) {
    v->capacity = (int)round((double)v->capacity * 1.5);
    v->arr = realloc(v->arr, v->capacity * sizeof(void*));
    if (v->arr == NULL) {
        perror("vec_grow() - realloc: out of memory");
    }
}

void vec_shrink(vec* v) {
    v->capacity = (int)max(round((double)v->capacity * 0.5), 1);
    v->arr = realloc(v->arr, v->capacity * sizeof(void*));
    if (v->arr == NULL) {
        perror("vec_shrink() - realloc: out of memory");
    }
}

vec vec_create(int capacity) {
    vec v = {0, capacity, 0};
    v.arr = malloc(capacity * sizeof(void*));
    if (v.arr == NULL) {
        perror("create_vec() - malloc: out of memory");
    }
    return v;
}

size_t vec_memory(vec* v) {
    return sizeof(void*) * v->capacity;
}

void vec_add(vec* v, void *element) {
    if (v->size + 1 > v->capacity) {
        vec_grow(v);
    }
    v->arr[v->size++] = element;
}

/*
 * removes the first element from the vector (element at index 0) and shifts the vector left.
 * returns the element removed.
 */
void *vec_remove(vec* v) {
    if (v->size == 0) return NULL;

    void *element = v->arr[0];

    // shift array
    for(int i = 0; i < v->size-1; i++) {
        v->arr[i] = v->arr[i+1];
    }

    // set last element to point to null
    v->arr[v->size-1] = NULL;
    v->size--;

    // check if we need to shrink the array
    if (v->size <= (int)round(v->capacity * 0.5) && v->size != 0) {
        vec_shrink(v);
    }

    return element;
}

void *vec_at(vec* v, int index) {
    if (index < 0 || index >= v->size) return NULL;
    return v->arr[index];
}

void vec_dump(char* buffer, vec* v) {
    sprintf(buffer, "vec<size=%d, capacity=%d, memory=%llu>", v->size, v->capacity, vec_memory(v));
}

void vec_free(vec* v) {
    if (v->arr) {
        free(v->arr);
    }
}