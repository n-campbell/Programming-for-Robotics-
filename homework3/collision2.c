#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "collision.h"

#define M_PI 3.14159265358979323846



/* bool contains(polygon_t *pg, double x, double y)
{
    // point is contained if it is on the "inside" of the four lines
    double z[pg->n_points];
    for (int i = 0; i < pg->n_points; i++) {
        z[i] = (pg->lines[i][2] - pg->lines[i][0])* (y - pg->lines[i][1]) -
                (x - pg->lines[i][0]) * (pg->lines[i][3] - pg->lines[i][1]);
        //printf("z[%d]: %lf \n", i, z[i]);
        //printf("x1, y1: %lf, %lf  x2,y2: %lf, %lf \n", pg->lines[i][2] - pg->lines[i][0], (pg->lines[i][3] - pg->lines[i][1]), 
       // (x - pg->lines[i][0]), y - pg->lines[i][1]); 

    }

    

    if(z[0] == 0){
        int pos_flag = 0;
        int neg_flag = 0;
        for(int i = 1; i < pg->n_points; i++){
            if(z[i] < 0 && pos_flag == 0){
                neg_flag = 1;
            }
            else if(z[i] < 0 && pos_flag == 1){
                return false;
            }
            else if(z[i] > 0 && neg_flag == 0){
                pos_flag = 1;
            }
            else if(z[i] > 0 && neg_flag == 1){
                return false;
            }
        }
    } else if (z[0] > 0){
        for(int i = 1; i < pg->n_points; i++){
            if(z[i] < 0){
                return false;
            }
        }
    } else if(z[0] < 0){
        for(int i = 1; i < pg->n_points; i++){
            if(z[i] > 0){
                return false;
            }
        }
    }

    return true;
} */

bool contains(polygon_t *pg, double x, double y) {
    // point is contained if it is on the "inside" of the four lines
    bool cw = true;
    int prod = 1;
    bool contains = true;
    bool non_zero = false;
    //double z;
    double z[pg->n_points];
    //find pairs of indices that make up each line in the polygon
    // a and b are consecutive points on the polygon

    for (int i = 0; i < pg->n_points; i++) {
        double a_x = pg->xs[i];
        double b_x = pg->xs[(i+1) % pg->n_points];
        double a_y = pg->ys[i];
        double b_y = pg->ys[(i+1) % pg->n_points];

        //points for cross product
        double x1 = a_x - x;
        double x2 = b_x - x;
        double y1 = a_y - y;
        double y2 = b_y - y;

        

        // cross product
        z[i] = x1 * y2 - x2 * y1;

        // check if ccw or cw (it will be ccw if z is positive)
        if (i == 0  || non_zero) {
            if (z[i] > 0) {
                cw = false;
            }
            if (z[i] == 0) {
                non_zero = true;
            }
            else{
                non_zero = false;
            }
        }

        if (cw) {
            if (z[i] > 0) {
                contains = false;
            }
        }

        // for clockwise, if the cross product is negative it is within the polygon
        else {
            if (z[i] < 0) {
                contains = false;
            }
        }
    }

    if ( contains ) {
        for (int i = 0; i < pg->n_points; i++) {

        printf("z[%d]: %lf \n", i, z[i]);

    }
    }

    return contains;
} 

bool lines_intersect(double x0, double y0, double x1, double y1,
                     double x2, double y2, double x3, double y3) {
    
    // using p0 = (x0,y0) as the origin where pi = (xi, yi)
    double z1 = (x1-x0)*(y2-y0) - (x2-x0)*(y1-y0);
    double z2 = (x1-x0)*(y3-y0) - (x3-x0)*(y1-y0);
    double z3 = (x3-x2)*(y0-y2) - (x0-x2)*(y3-y2);
    double z4 = (x3-x2)*(y1-y2) - (x1-x2)*(y3-y2);
    
    // if both cross products have opposite signs, they intersect
    if ( (z1*z2 < 0) && (z3*z4 < 0) ) {
        return true;
    }
    else {
        return false;
    }
}

void set_lines(polygon_t *pg) {
    for (int i = 0; i < pg->n_points; i++) {
        pg->lines[i][0] = pg->xs[i];
        pg->lines[i][1] = pg->ys[i];
        pg->lines[i][2] = pg->xs[(i+1) % pg->n_points];
        pg->lines[i][3] = pg->ys[(i+1) % pg->n_points];
    }
}

bool check_intersection(polygon_t *pg1, polygon_t *pg2) {
    bool any_intersection = false;
    bool intersection = false;
    set_lines(pg1);
    set_lines(pg2);
    //for each line, l1,  in polygon 1
    for (int l1 = 0; l1 < pg1->n_points; l1 ++) {
        // for each line, l2,  in polygon 2
        for (int l2 = 0; l2 < pg2->n_points; l2++) {
                    
            intersection = lines_intersect(pg1->lines[l1][0],pg1->lines[l1][1],pg1->lines[l1][2],pg1->lines[l1][3],pg2->lines[l2][0],pg2->lines[l2][1],pg2->lines[l2][2],pg2->lines[l2][3]);

            // if both cross product checks have opposite signs then lines intersect
            if ( intersection ) {
                any_intersection = true;
                //printf("line intersection between ");
            }
/*             else {
                printf("NO line intersection between ");
            }
            printf("(%.5lf, %.5lf), (%.5lf, %.5lf) and (%.5lf, %.5lf), (%.5lf, %.5lf)\n", 
                    pg1->lines[l1][0],pg1->lines[l1][1],pg1->lines[l1][2],pg1->lines[l1][3],
                    pg2->lines[l2][0],pg2->lines[l2][1],pg2->lines[l2][2],pg2->lines[l2][3]);;*/
        } 
    }
    return any_intersection;
}

bool check_collides(polygon_t *pg1, polygon_t *pg2) {
    bool intersects = check_intersection(pg1, pg2);
    bool contains1 = false;
    for (int i = 0; i < pg2->n_points; i++) {
        if (contains(pg1, pg2->xs[i], pg2->ys[i])) {
            contains1 = true;
        }
    }
    bool contains2 = false;
    for (int i = 0; i < pg1->n_points; i++) {
        if (contains(pg2, pg1->xs[i], pg1->ys[i])) {
            contains2 = true;
        }
    }
    if (contains1) {
        //printf("polygon1 contains polygon2\n");
    }
    if (contains2) {
        //printf("polygon2 contains polygon1\n");
    }
    return intersects || contains1 || contains2;
}

polygon_t parse_polygon(FILE *f) {
    polygon_t pg = {0};
    int read_global = fscanf(f, "%lf %lf %lf %d", &pg.x,&pg.y, &pg.rot, &pg.n_points);
    
    if ( pg.n_points > 16) {
        fprintf(stderr, "File contains more than 16 points. \n ");
        perror("");
        exit(1); 
    }

    if (read_global !=4) {
        fprintf(stderr, "could not read values!\n ");
        perror("");
        exit(1); 
    }

    for (int i = 0; i < pg.n_points; i++) {
        int read_xs = fscanf(f, "%lf", &pg.xs[i]);
        if (read_xs !=1) {
            fprintf(stderr, "could not read values!\n ");
            exit(1);
        }
    }

    for (int i = 0; i < pg.n_points; i++) {
        int read_ys = fscanf(f, "%lf", &pg.ys[i] );
        if (read_ys !=1) {
            fprintf(stderr, "could not read values!\n ");
            exit(1);
        }
    }

    // rotate 
    for (int i = 0; i < pg.n_points; i++) {
        double temp_x = pg.xs[i];
        pg.xs[i] = pg.xs[i] * cos((M_PI * pg.rot) / 180) - pg.ys[i] * sin((M_PI * pg.rot) /180);
        pg.ys[i] = temp_x * sin((M_PI * pg.rot) / 180) + pg.ys[i] * cos((M_PI * pg.rot) /180);
    }
    
    // translate
    for (int i = 0; i < pg.n_points; i++) {
        pg.xs[i] = pg.xs[i] + pg.x;
        pg.ys[i] = pg.ys[i] + pg.y;
    }

    return pg;
}
