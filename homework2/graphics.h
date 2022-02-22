#pragma once

#include "bmp.h"
#include "vector_xy_t.h"
#include "vector_xy_i32_t.h"

 //----- Test 1 -----
void gx_clear(bitmap_t *bmp, color_bgr_t color);

//----- Test 2 -----
//void gx_rasterize_line(int x0, int y0, int x1, int y1, vector_xy_i32_t * vec);
vector_xy_i32_t * gx_rasterize_line(int x0, int y0, int x1, int y1, vector_xy_i32_t * vec);

void gx_draw_line(bitmap_t *bmp, color_bgr_t color, int x0, int y0, int x1, int y1);


//----- Test 3 -----
//void gx_rect(double width, double height, vector_xy_t * points);
vector_xy_t * gx_rect(double width, double height, vector_xy_t * points);

void gx_round_points(vector_xy_t *points);

void gx_draw(bitmap_t *bmp, color_bgr_t color, vector_xy_t *points);


//----- Test 4, 5, 6 -----
//void gx_trans(double x, double y, vector_xy_t *points);
vector_xy_t *  gx_trans(double x, double y, vector_xy_t *points);


//----- Test 7 -----
void gx_fill(bitmap_t *bmp, color_bgr_t color, vector_xy_t *points);


//----- Test 8 -----
//void gx_robot(double dim, vector_xy_t * points);
vector_xy_t * gx_robot(double dim, vector_xy_t * points);

//----- Test 9 -----
//void gx_rot(double theta, vector_xy_t *points);
vector_xy_t * gx_rot(double theta, vector_xy_t *points);

 