#include "graphics.h"
#include "vector_xy_i32_t.h"
#include "vector_xy_t.h"

#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


static int min(int a, int b) {
    return a < b ? a : b;
}

static int max(int a, int b) {
    return a > b ? a : b;
}

 //----- Test 1 -----
void gx_clear(bitmap_t *bmp, color_bgr_t color) {
/*     for (int i = 0; i < (bmp->height * bmp-> width); i++) {
        bmp->data[i] = color;
    } */

    for (int y = 0; y < bmp->height; y++) {
        for (int x = 0; x < bmp->width; x++) {
            bmp->data[ y * bmp->width + x ] = color;
        }
    }  
}


//----- Test 2 -----
vector_xy_i32_t * gx_rasterize_line(int x0, int y0, int x1, int y1, vector_xy_i32_t * vec) {
    // Bresenham's Line Drawing, as applied to rasterizing a convex polygon
    //vector_xy_i32_t *vec = vector_xy_i32_create();

    int dx = abs( x1 - x0 );
    int sx = 0;
    if ( x0 != x1 ) {
        if ( x0 < x1 ) {
            sx = 1;
        }
        else {
            sx = -1;
        }
    }

    int dy = - abs( y1 - y0 );
    int sy = 0;
    if ( y0 != y1 ) {
        if ( y0 < y1 ) {
            sy = 1;
        }
        else {
            sy = -1;
        }
    }

    int err = dx + dy;
    int e2;
    xy_i32_t points = {0};
    while ( true ) {
        points.x = x0;
        points.y = y0;
        vector_xy_i32_append(vec, points);
        if ( x0 == x1 && y0 == y1 ) {
            break;
        }
        e2 = 2*err;
        if ( e2 >=  dy) {
            err += dy;
            x0 += sx;
        }
        if ( e2 <= dx ) {
            err += dx;
            y0 += sy;
        }
    }
    return vec;
}

void gx_draw_line(bitmap_t *bmp, color_bgr_t color, int x0, int y0, int x1, int y1) {
    vector_xy_i32_t *points5 = vector_xy_i32_create();
    points5 = gx_rasterize_line(x0, y0, x1, y1, points5);
    for ( int i = 0; i < points5->size; i++ ) {
        if (points5->data[i].x >= 0 && points5->data[i].y >= 0 ) {
            bmp->data[points5->data[i].y * bmp->width + points5->data[i].x ] = color;
        }
    }
    vector_xy_i32_destroy(points5);
}


//----- Test 3 -----
vector_xy_t * gx_rect(double width, double height, vector_xy_t * points) {

    xy_t point1;
    point1.x = - width/2;
    point1.y = - height/2;
    vector_xy_append( points , point1 );

    xy_t point2;
    point2.x = width/2;
    point2.y = - height/2;
    vector_xy_append( points , point2 );

    xy_t point3;
    point3.x = width/2;
    point3.y = height/2;
    vector_xy_append( points , point3 );

    xy_t point4;
    point4.x = - width/2;
    point4.y = height/2;
    vector_xy_append( points , point4 );

    return points;
}

void gx_round_points(vector_xy_t *points) {
/*     // print before rounding 
    for ( int i = 0; i < points->size; i++ ) {
        printf("Before rounding: %.5f %.5f\n", points->data[i].x, points->data[i].y );
    } */

    // find minimum x in polygon 
    double x_min = points->data[0].x;
    for ( int i = 1; i < points->size; i++ ) {
        if ( points->data[i].x < x_min ) {
            x_min = points->data[i].x;
        }
    }

    // find minimum y in polygon
    double y_min = points->data[0].y;
    for ( int i = 1; i < points->size; i++ ) {
        if (  points->data[i].y < y_min) {
            y_min = points->data[i].y;
        }
    }

    // round 
    for ( int i = 0; i < points->size; i++ ) {
        if ( points->data[i].x == x_min ) {
            points->data[i].x = ceil( points->data[i].x );
        }
        else {
            points->data[i].x = floor( points->data[i].x - 1e-6 );
        }
        if ( points->data[i].y == y_min) {
            points->data[i].y = ceil( points->data[i].y );
        }
        else {
            points->data[i].y = floor( points->data[i].y - 1e-6 );
        }
    }

/*     // print after rounding
    for ( int i = 0; i < points->size; i++ ) {
            printf("After rounding: %.5f %.5f\n", points->data[i].x, points->data[i].y);
    } */

    // add first point to end to make closed shape

    //return points;
} 

void gx_draw(bitmap_t *bmp, color_bgr_t color, vector_xy_t *points) {
    gx_round_points(points);
    vector_xy_append( points , points->data[0] );
    double x0;
    double x1;
    double y0;
    double y1;
    for (int i = 0; i < points->size - 1 ; i++) {
        x0 = points->data[i].x;
        x1 = points->data[i+1].x;
        y0 = points->data[i].y;
        y1 = points->data[i+1].y;
        gx_draw_line(bmp, color, (int)x0, (int)y0, (int)x1, (int)y1);
    }

    
}

//----- Test 4, 5, 6 -----
vector_xy_t *  gx_trans(double x, double y, vector_xy_t *points) {
    for ( int i = 0; i < points->size; i++ ) {
        points->data[i].x = points->data[i].x + x;
        points->data[i].y = points->data[i].y + y;
    }
    return points;
}


//----- Test 7 -----
// rectangle rasterization:
// https://stackoverflow.com/questions/10061146/how-to-rasterize-rotated-rectangle-in-2d-by-setpixel
void gx_fill(bitmap_t *bmp, color_bgr_t color, vector_xy_t *points) {
    gx_round_points(points);
    vector_xy_append( points , points->data[0] );

    int *buff_x0 = malloc(sizeof(int) * bmp->height);
    int *buff_x1 = malloc(sizeof(int) * bmp->height);

    // initialize the buffers to -1
    for (int i = 0; i < bmp->height; i++) {
        buff_x0[i] = -1;
        buff_x1[i] = -1;
    }
    
    // Iterate through each point (x, y) of each rasterized line, 
    // finding the minimum and maximum values of x for each value of y.

    int x;
    int y;

    for (int i = 0; i < points->size - 1 ; i++) {
        vector_xy_i32_t * pixels = vector_xy_i32_create();
        gx_rasterize_line( (int)points->data[i].x, (int)points->data[i].y, (int)points->data[i+1].x, (int)points->data[i+1].y, pixels);
        for (int j = 0; j < pixels->size; j++) {
            x = pixels->data[j].x;
            y = pixels->data[j].y;
            if (buff_x0[y] == -1) {
                buff_x0[y] = x;
                buff_x1[y] = x;
            }
            else {
                buff_x0[y] = min( buff_x0[y], x );
                buff_x1[y] = max( buff_x1[y], x );
            }
        }
        vector_xy_i32_destroy(pixels);
    }

    for (int y = 0; y < bmp->height; y++) {
        if ( buff_x0[y] != -1 && buff_x1[y] != -1 ) {
            for (int start = buff_x0[y]; start <= buff_x1[y]; start++)
                bmp->data[ y * bmp->width + start ] = color;
            //gx_draw_line(bmp, color, (int)buff_x0[y], (int)y, (int)buff_x1[y], (int)y);
        }
    }
    
    free(buff_x0);
    free(buff_x1);
}


//----- Test 8 -----
vector_xy_t * gx_robot(double dim, vector_xy_t * points) {
    // dim = 20
    //vector_xy_t *points = vector_xy_create();
    xy_t vertex = {0};

    vertex.x = - dim * ( 4.0 / 6 );
    vertex.y = - dim / 2;
    vector_xy_append( points , vertex );

    vertex.x = dim * ( 4.0 / 6 );
    vertex.y = 0.0;
    vector_xy_append( points , vertex );

    vertex.x = - dim * ( 4.0 / 6 );
    vertex.y =  dim / 2;
    vector_xy_append( points , vertex );

    return points;
}


//----- Test 9 -----
vector_xy_t * gx_rot(double theta, vector_xy_t *points) {
    double temp_x;
     for ( int i = 0; i < points->size; i++ ) {
        temp_x = points->data[i].x;
        points->data[i].x = points->data[i].x * cos(theta) + points->data[i].y * sin(theta);
        points->data[i].y = - temp_x * sin(theta) + points->data[i].y * cos(theta);
    } 
    return points;
} 

