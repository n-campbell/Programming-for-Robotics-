#include "collision.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#define M_PI 3.14159265358979323846


bool contains(polygon_t *pg, double x, double y)
{
    // point is contained if it is on the "inside" of the four lines
    double z[pg->n_points];
    for (int i = 0; i < pg->n_points; i++) {
        z[i] = (pg->lines[i][2] - pg->lines[i][0])* (y - pg->lines[i][1]) -
                (x - pg->lines[i][0]) * (pg->lines[i][3] - pg->lines[i][1]);
        //printf("z[%d]: %lf \n", i, z[i]);
        //printf("x1, y1: %lf, %lf  x2,y2: %lf, %lf \n", pg->lines[i][2] - pg->lines[i][0], (pg->lines[i][3] - pg->lines[i][1]), 
        //(x - pg->lines[i][0]), y - pg->lines[i][1]); 

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
}

void compute_lines(polygon_t *pg){
    for (int i = 0; i < pg->n_points; i++) {
        pg->lines[i][0] = pg->xs[i];
        pg->lines[i][1] = pg->ys[i];
        // after adding 1, we get the remainder, which causes n_points to wrap around to 0
        pg->lines[i][2] = pg->xs[(i + 1) % pg->n_points];
        pg->lines[i][3] = pg->ys[(i + 1) % pg->n_points];
    }
}

bool lines_intersect(double x0, double y0, double x1, double y1,
                     double x2, double y2, double x3, double y3)
{
    double z1 = (x1 - x0)*(y2 - y0) - (x2 - x0)*(y1 - y0);
    double z2 = (x1 - x0)*(y3 - y0) - (x3 - x0)*(y1 - y0);
    double z3 = (x3 - x2)*(y0 - y2) - (x0 - x2)*(y3 - y2);
    double z4 = (x3 - x2)*(y1 - y2) - (x1 - x2)*(y3 - y2);
    if(z1*z2 < 0 && z3*z4 < 0){
        return true;
    }
    else{
        return false;
    }
}

bool check_intersection(polygon_t *pg1, polygon_t *pg2)
{
    bool any_intersection = false;
    bool intersect = false;
    compute_lines(pg1);
    compute_lines(pg2);
    for(int i = 0; i < pg1->n_points; i++){
        for(int j = 0; j < pg2->n_points; j++){
            intersect = lines_intersect(pg1->lines[i][0], pg1->lines[i][1], pg1->lines[i][2], pg1->lines[i][3],
                                                pg2->lines[j][0], pg2->lines[j][1], pg2->lines[j][2], pg2->lines[j][3]);
        if(intersect == true){
            any_intersection = true;
        } else {

        }   
        }
    }
    return any_intersection;
}

bool check_collides(polygon_t *pg1, polygon_t *pg2)
{
    bool intersects = check_intersection(pg1, pg2);
    bool contains1 = false;
    for (int i = 0; i < pg2->n_points; i++)
    {
        if (contains(pg1, pg2->xs[i], pg2->ys[i]))
        {
            contains1 = true;
            break;
        }
    }
    if(contains1){
    }
    bool contains2 = false;
    for (int i = 0; i < pg1->n_points; i++)
    {
        if (contains(pg2, pg1->xs[i], pg1->ys[i]))
        {
            contains2 = true;
            break;
        }
    }
    if(contains2){
    }
    return intersects || contains1 || contains2;
}


