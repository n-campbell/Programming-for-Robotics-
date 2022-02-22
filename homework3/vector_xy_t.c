#include "vector_xy_t.h"

#include <stdlib.h>
#include <stdio.h>

vector_xy_t *vector_xy_create(void) {
    vector_xy_t *vec = malloc(sizeof(vector_xy_t));
    vec->size = 0; 
    vec->capacity = 4; 
    vec->data = malloc(sizeof(*vec->data) * vec->capacity);
	return vec;
}

void vector_xy_destroy(vector_xy_t *vec) {
    free(vec->data);
	free(vec);
    //vec->data = NULL;
    //vec->size = 0;
    //vec->capacity = 0;
    
}

void vector_xy_append(vector_xy_t *vec, xy_t value) {
    // reallocate space if necessary 
	vector_xy_t * temp = vec;
    if (vec->size + 1 == vec->capacity) {
        vec->capacity *= 2;
        vec->data = realloc(temp->data, sizeof(*temp->data) * temp->capacity);
    }
    // append value
    vec->data[vec->size].x = value.x;
	vec->data[vec->size].y = value.y;
    vec->size++;
    
}
