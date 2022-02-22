#include "vector_xy_i32_t.h"

#include <stdlib.h>

vector_xy_i32_t *vector_xy_i32_create(void) {
    vector_xy_i32_t *vec = malloc(sizeof(vector_xy_i32_t));
    vec->size = 0; 
    vec->capacity = 2; 
    vec->data = malloc(sizeof(*vec->data) * vec->capacity);
	return vec;
}

void vector_xy_i32_destroy(vector_xy_i32_t *vec) {
    free(vec->data);
    free(vec);
}

void vector_xy_i32_append(vector_xy_i32_t *vec, xy_i32_t value) {
    // reallocate space if necessary 
    vector_xy_i32_t *temp = vec;
    if (vec->size + 1 == vec->capacity) {
        vec->capacity *= 2;
        vec->data = realloc(temp->data, sizeof(*temp->data) * temp->capacity);
    }
    // append value
    vec->data[vec->size].x = value.x;
	vec->data[vec->size].y = value.y;
    vec->size++;
}
