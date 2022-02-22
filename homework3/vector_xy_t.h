#pragma once

#include <inttypes.h>

typedef struct xy {
    double x;
    double y;
} xy_t;

typedef struct vector_xy {
    uint32_t size;
    uint32_t capacity;
    xy_t *data;
} vector_xy_t;

vector_xy_t *vector_xy_create(void);

void vector_xy_destroy(vector_xy_t *vec);

void vector_xy_append(vector_xy_t *vec, xy_t value);
