#pragma once

#include <inttypes.h>

typedef struct xy_i32 {
    int32_t x;
    int32_t y;
} xy_i32_t;

typedef struct vector_xy_i32 {
    uint32_t size;
    uint32_t capacity;
    xy_i32_t *data;
} vector_xy_i32_t;

vector_xy_i32_t *vector_xy_i32_create(void);

void vector_xy_i32_destroy(vector_xy_i32_t *vec);

void vector_xy_i32_append(vector_xy_i32_t *vec, xy_i32_t value);