#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "image_server.h"
#include "graphics.h"
#include "bmp.h"
#include "vector_xy_t.h"
#include "vector_xy_i32_t.h"

#define M_PI 3.14159265358979323846

void draw_case(bitmap_t *bmp, int test_case) {
    color_bgr_t white = {255, 255, 255};
    color_bgr_t red = {0, 0, 255};
    color_bgr_t black = {0, 0, 0};
    vector_xy_t *shape;

    switch (test_case) {
    case 0:
        bmp->width = 64; 
        bmp->height = 48;
        bmp->data = calloc(bmp->width * bmp->height, sizeof(color_bgr_t));
        break;
    case 1:
        bmp->width = 320; 
        bmp->height = 240;
        bmp->data = calloc(bmp->width * bmp->height, sizeof(color_bgr_t));
        gx_clear(bmp, red);
        break;
    case 2:
        bmp->width = 640;
        bmp->height = 480;
        bmp->data = calloc(bmp->width * bmp->height, sizeof(color_bgr_t));
        gx_clear(bmp, black);
        gx_draw_line(bmp, white, 10, 10, 200, 200);
        break;
    case 3:
        bmp->width = 640;
        bmp->height = 480;
        bmp->data = calloc(bmp->width * bmp->height, sizeof(color_bgr_t));
        gx_clear(bmp, black);
        vector_xy_t * points1 = vector_xy_create();
        gx_rect(4, 4, points1);

        // print before rounding 
        for ( int i = 0; i < points1->size; i++ ) {
            printf("Before rounding: %.5f %.5f \n", points1->data[i].x, points1->data[i].y );
        } 

        gx_draw(bmp, white, points1);

        //print after rounding 
        for ( int i = 0; i < points1->size - 1; i++ ) {
            printf("After rounding: %.5f %.5f \n", points1->data[i].x, points1->data[i].y);
        } 

        vector_xy_destroy(points1);
        break;
    case 4:
        bmp->width = 640;
        bmp->height = 480;
        bmp->data = calloc(bmp->width * bmp->height, sizeof(color_bgr_t));
        gx_clear(bmp, black);
        vector_xy_t * points2 = vector_xy_create();
        gx_rect(4,4, points2);
        gx_trans(2,2, points2);

        // print before rounding 
        for ( int i = 0; i < points2->size; i++ ) {
            printf("Before rounding: %.5f %.5f\n", points2->data[i].x, points2->data[i].y );
        } 

        gx_draw(bmp, white, points2);

        //print after rounding 
        for ( int i = 0; i < points2->size - 1; i++ ) {
            printf("After rounding: %.5f %.5f\n", points2->data[i].x, points2->data[i].y);
        } 
        vector_xy_destroy(points2);
        break;
    case 5:
        bmp->width = 640;
        bmp->height = 480;
        bmp->data = calloc(bmp->width * bmp->height, sizeof(color_bgr_t));
        gx_clear(bmp, black);
        vector_xy_t * points3 = vector_xy_create();
        gx_rect(5, 5, points3);
        gx_trans(2, 2, points3);

        //print before rounding 
        for ( int i = 0; i < points3->size - 1; i++ ) {
            printf("Before rounding: %.5f %.5f\n", points3->data[i].x, points3->data[i].y);
        } 

        gx_draw(bmp, white, points3);

        //print after rounding 
        for ( int i = 0; i < points3->size - 1; i++ ) {
            printf("After rounding: %.5f %.5f\n", points3->data[i].x, points3->data[i].y);
        } 
        vector_xy_destroy(points3);
        break;
    case 6:
        bmp->width = 640;
        bmp->height = 480;
        bmp->data = calloc(bmp->width * bmp->height, sizeof(color_bgr_t));
        gx_clear(bmp, black);
        vector_xy_t * boarder = vector_xy_create();
        gx_rect(600, 440, boarder);
        gx_trans(640/2, 480/2, boarder);

        //print before rounding 
        for ( int i = 0; i < boarder->size - 1; i++ ) {
            printf("Before rounding: %.5f %.5f\n", boarder->data[i].x, boarder->data[i].y);
        } 

        gx_draw(bmp, white, boarder);

        //print after rounding 
        for ( int i = 0; i < boarder->size - 1; i++ ) {
            printf("After rounding: %.5f %.5f\n", boarder->data[i].x, boarder->data[i].y);
        } 
        vector_xy_destroy(boarder);
        break;
    case 7:
        bmp->width = 640;
        bmp->height = 480;
        bmp->data = calloc(bmp->width * bmp->height, sizeof(color_bgr_t));
        gx_clear(bmp, black);
        vector_xy_t * fill_boarder = vector_xy_create();
        gx_rect(600,440, fill_boarder);
        gx_trans(640/2,480/2, fill_boarder);

        //print before rounding 
        for ( int i = 0; i < fill_boarder->size - 1; i++ ) {
            printf("Before rounding: %.5f %.5f\n", fill_boarder->data[i].x, fill_boarder->data[i].y);
        } 
        gx_fill(bmp, white, fill_boarder);

        //print after rounding 
        for ( int i = 0; i < fill_boarder->size - 1; i++ ) {
            printf("After rounding: %.5f %.5f\n", fill_boarder->data[i].x, fill_boarder->data[i].y);
        } 
        vector_xy_destroy(fill_boarder);
        break;
    case 8:
        bmp->width = 640;
        bmp->height = 480;
        bmp->data = calloc(bmp->width * bmp->height, sizeof(color_bgr_t));
        gx_clear(bmp, black);
        vector_xy_t * rob_points = vector_xy_create();
        gx_robot(20, rob_points );
        gx_trans(400,400, rob_points);

        //print before rounding 
        for ( int i = 0; i < rob_points->size - 1; i++ ) {
            printf("Before rounding: %.5f %.5f\n", rob_points->data[i].x, rob_points->data[i].y);
        } 

        gx_fill(bmp, white, rob_points);

        //print after rounding 
        for ( int i = 0; i < rob_points->size - 1; i++ ) {
            printf("After rounding: %.5f %.5f\n", rob_points->data[i].x, rob_points->data[i].y);
        } 
        vector_xy_destroy(rob_points);
        break;
    case 9:
        bmp->width = 640;
        bmp->height = 480;
        bmp->data = calloc(bmp->width * bmp->height, sizeof(color_bgr_t));
        gx_clear(bmp, black);
        vector_xy_t * robot = vector_xy_create();
        gx_robot(20, robot);
        gx_rot(M_PI/6, robot);
        gx_trans(400,400, robot);

        //print before rounding 
        for ( int i = 0; i < robot->size - 1; i++ ) {
            printf("Before rounding: %.5f %.5f\n", robot->data[i].x, robot->data[i].y);
        } 

        gx_fill(bmp, white, robot);

        //print after rounding 
        for ( int i = 0; i < robot->size - 1; i++ ) {
            printf("After rounding: %.5f %.5f\n", robot->data[i].x, robot->data[i].y);
        } 
        vector_xy_destroy(robot);
        break;
    default:
        break;
    }
}

int main(int argc, char **argv) {
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "usage: %s <test case> [<port number>]\n", argv[0]);
        return 1;
    }

    char *port = "8000";
    if (argc == 3) {
        port = argv[2]; 
    }

    bitmap_t bmp = { 0 };

    int test_case = atoi(argv[1]);
    if (test_case < 0 || test_case > 9) {
        free(bmp.data);
        fprintf(stderr, "didn't recognize test case %s\n", argv[1]);
        return 1;
    }

    draw_case( &bmp , test_case );
    size_t image_size = bmp_calculate_size(&bmp);
    uint8_t *image_data = malloc(image_size); // serialized bitmap data
    bmp_serialize(&bmp, image_data);  
 
    image_server_set_data(image_size, image_data);
    image_server_start(port);
/*  
    FILE *f = fopen("my_image.bmp", "wb");
    fwrite(image_data, image_size, 1, f);
    fclose(f);    
    */
    sleep(1);

    // free memory 
    free(image_data);
    //image_data = NULL;

    free(bmp.data);
    //bmp.data = NULL;


    return 0;
}