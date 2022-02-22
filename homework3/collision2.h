#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

typedef struct polygon {
    int n_points;
    double x;
    double y;
    double rot;
    double xs[16];
    double ys[16];
    double lines[16][16];
    double z[16];
} polygon_t;

bool contains(polygon_t *pg, double x, double y);

bool lines_intersect(double x0, double y0, double x1, double y1,
                     double x2, double y2, double x3, double y3);

void set_lines(polygon_t *pg);

bool check_intersection(polygon_t *pg1, polygon_t *pg2);

bool check_collides(polygon_t *pg1, polygon_t *pg2);

polygon_t parse_polygon(FILE *f);

