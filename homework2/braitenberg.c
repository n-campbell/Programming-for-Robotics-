#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h> 

#include "image_server.h"
#include "graphics.h"
#include "bmp.h"
#include "vector_xy_i32_t.h"
#include "vector_xy_t.h"
#include "collision.h"

#define WIDTH 640
#define HEIGHT 480

#define N_LAMPS 3

#define MAX_DIST 12

#define ROBOT_BASE 80

#define M_PI 3.14159265358979323846



typedef struct state {
    int time_step;
    bitmap_t bmp;
    size_t image_size;
    uint8_t *image_data;

    double x;
    double y;
    double theta;
} state_t;


void get_lamp_coords(vector_xy_t *coords) {

    xy_t point = {0};

    point.x = 124.1;
    point.y = 224.1;
    vector_xy_append(coords, point);

    point.x = 349.1;
    point.y = 99.1;
    vector_xy_append(coords, point);

    point.x = 449.1;
    point.y = 349.1;
    vector_xy_append(coords, point);

}

void get_lamp_vertices(vector_xy_t *lamp_points, int lamp_num) {

    vector_xy_t *coords = vector_xy_create();
    get_lamp_coords(coords);

    lamp_points = gx_rect(12, 12, lamp_points);
    lamp_points = gx_rot(M_PI/4, lamp_points);
    lamp_points = gx_trans(coords->data[lamp_num].x, coords->data[lamp_num].y, lamp_points);

    vector_xy_destroy(coords);
}

void get_robot_coords(state_t *state, vector_xy_t *robot_points) {
    robot_points = gx_robot(20, robot_points);
    robot_points = gx_rot(state->theta, robot_points);
    robot_points = gx_trans(state->x, state->y, robot_points);
}

void print_points(state_t *state){
    vector_xy_t *r_points = vector_xy_create();
    get_robot_coords(state, r_points);
    //printf("r_points size: %d\n", r_points->size);
    //printf("state->time->step")
    for (int i = 0; i < r_points->size; i++) {
        printf("%d: Before rounding: %.5f %.5f\n", state->time_step, r_points->data[i].x, r_points->data[i].y);
    }
    gx_round_points(r_points);
    for (int i = 0; i < r_points->size; i++) {
        printf("%d: After rounding: %.5f %.5f\n", state->time_step, r_points->data[i].x, r_points->data[i].y);
    }
    vector_xy_destroy(r_points);
}

void update_collisions(state_t *state) {

    int collision = 0;

    polygon_t pg_robot = {0};
    vector_xy_t *robot_vertices = vector_xy_create();
    get_robot_coords(state, robot_vertices);
    pg_robot.n_points = robot_vertices->size;

    for (int i = 0; i < robot_vertices->size; i++) {
        pg_robot.xs[i] = robot_vertices->data[i].x;
        pg_robot.ys[i] = robot_vertices->data[i].y;
    }

    vector_xy_destroy(robot_vertices);

    for (int i = 0; i < N_LAMPS; i++) {

        polygon_t pg_lamp = {0};
        vector_xy_t *lamp_vertices = vector_xy_create();
        get_lamp_vertices(lamp_vertices, i);
        pg_lamp.n_points = lamp_vertices->size;

        for (int j=0; j < lamp_vertices->size; j++) {
            pg_lamp.xs[j] = lamp_vertices->data[j].x;
            pg_lamp.ys[j] = lamp_vertices->data[j].y;
        }

        
        while (check_collides(&pg_robot, &pg_lamp)) {

            // brute-force move out of the way:
            //   the robot should move a total of 0.5 pixel distance away 
            //   from the obstacle along the vector between the robot and that obstacle’s center

            collision = 1;
            // create vector from robot center to lamp center
            xy_t dir_vec;
            vector_xy_t *lamp_origins = vector_xy_create();
            get_lamp_coords(lamp_origins);

            double dir_vec_mag = sqrt( pow(lamp_origins->data[i].x - state->x , 2) + pow(lamp_origins->data[i].y - state->y, 2) );
            dir_vec.x = ( lamp_origins->data[i].x - state->x ) / dir_vec_mag; // pointing from robot to lamp
            dir_vec.y = - ( lamp_origins->data[i].y - state->y ) / dir_vec_mag;
            
            state->x -= 0.5 * dir_vec.x;
            state->y += 0.5 * dir_vec.y;

            // update robot polygon 
            vector_xy_t *robot_vertices_ = vector_xy_create();
            get_robot_coords(state, robot_vertices_);

            for (int i = 0; i < robot_vertices_->size; i++) {
                pg_robot.xs[i] = robot_vertices_->data[i].x;
                pg_robot.ys[i] = robot_vertices_->data[i].y;
            }

            vector_xy_destroy(robot_vertices_);

            vector_xy_destroy(lamp_origins);
        }

        vector_xy_destroy(lamp_vertices);
    }

    if (collision == 1) {
        printf("%d: Lamp collision\n", state->time_step);
    }

} 


void update_movement(state_t *state) {

    double left_dist = 0;
    double right_dist = 0;
    double lamp_power = 100000;
    double track_width = 80;
    double max_movement = 12;
    double dist_sq;
    double theta;
    double forward_dist;
    xy_t dir={0};
    xy_t eye_l={0};
    xy_t eye_r={0};

    vector_xy_t *lamp_points = vector_xy_create();
    get_lamp_coords(lamp_points);

    for (int i = 0; i < N_LAMPS; i++) {
        dist_sq =  pow(lamp_points->data[i].x - state->x , 2) + pow(lamp_points->data[i].y - state->y, 2);  // squared distance from robot to lamp l
        dir.x =  ( lamp_points->data[i].x - state->x ) / sqrt(dist_sq) ;  // unit vector pointing from robot to lamp l
        dir.y = ( lamp_points->data[i].y - state->y ) / sqrt(dist_sq);
        eye_l.x = cos(state->theta - (M_PI / 3)); // unit vector pointing at angle theta - 60 degrees
        eye_l.y = - sin(state->theta - (M_PI / 3));
        eye_r.x = cos(state->theta + (M_PI / 3)); //unit vector pointing at angle theta + 60 degrees
        eye_r.y = - sin(state->theta +  (M_PI / 3)); 

        left_dist = left_dist + fmax( 0.0, dir.x * eye_r.x + dir.y * eye_r.y ) * lamp_power / dist_sq;
        right_dist = right_dist + fmax( 0.0, dir.x * eye_l.x + dir.y * eye_l.y) * lamp_power / dist_sq;

    }

    left_dist = fmin(max_movement, left_dist);
    right_dist = fmin(max_movement, right_dist);
    state->theta += (left_dist - right_dist) / ROBOT_BASE;

    //theta += (left_dist - right_dist) / track_width;
    forward_dist = (left_dist + right_dist) / 2;

    // move robot forward_dist in the theta direction
    state->x += forward_dist * cos(state->theta);
    state->y -= forward_dist * sin(state->theta);

    vector_xy_destroy(lamp_points);
} 

void update_graphics(state_t *state) {

    color_bgr_t white = {255, 255, 255};
    color_bgr_t red = {0, 0, 255};
    color_bgr_t black = {0, 0, 0};
    color_bgr_t green = {0, 255, 0};
    color_bgr_t yellow = {0, 255, 255};

    bitmap_t *b = &state->bmp;
    //b->data = calloc(WIDTH * HEIGHT, sizeof(color_bgr_t));

    // black background with white boarder
    gx_clear(b, black);
    vector_xy_t * boarder_points = vector_xy_create();
    boarder_points = gx_rect(600, 440, boarder_points );
    boarder_points = gx_trans(WIDTH/2, HEIGHT/2, boarder_points );
    gx_draw(b, white, boarder_points);
    vector_xy_destroy(boarder_points);
    

    // draw lamps 
    vector_xy_t * lamp1 = vector_xy_create();
    lamp1 = gx_rect(12, 12, lamp1);
    lamp1 =gx_rot(M_PI/4, lamp1);
    lamp1 = gx_trans(124.1, 224.1, lamp1);
    gx_fill(b, yellow, lamp1);
    vector_xy_destroy(lamp1);
    

    vector_xy_t * lamp2 = vector_xy_create();
    lamp2 = gx_rect(12, 12, lamp2);
    lamp2 = gx_rot(M_PI/4, lamp2);
    lamp2 = gx_trans(349.1, 99.1, lamp2);
    gx_fill(b, yellow, lamp2);
    vector_xy_destroy(lamp2);
    

    vector_xy_t * lamp3 = vector_xy_create();
    lamp3 = gx_rect(12, 12, lamp3);
    lamp3 = gx_rot(M_PI/4, lamp3);
    lamp3 = gx_trans(449.1, 349.1, lamp3);
    gx_fill(b, yellow, lamp3);
    vector_xy_destroy(lamp3);
    

    // draw robot 
    vector_xy_t * robot_vec = vector_xy_create();
    robot_vec = gx_robot(20, robot_vec);
    robot_vec = gx_rot(state->theta, robot_vec);
    robot_vec = gx_trans(state->x, state->y, robot_vec);
    gx_fill(b, green, robot_vec);
    vector_xy_destroy(robot_vec); 

 
    state->image_size = bmp_calculate_size(b);
    state->image_data = malloc(state->image_size); 

    bmp_serialize(&state->bmp, state->image_data);
    image_server_set_data(state->image_size, state->image_data); 

}

int main(int argc, char **argv) {
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "usage: %s <time steps> <fast=0|1> [<port number=8000>] \n", argv[0]);
        return 1;
    }

    char *port = "8000";
    if (argc == 4) {
        port = argv[3];
    }

    int time_steps = atoi(argv[1]);
    int fast = atoi(argv[2]);

    bitmap_t bmp = { 0 };
    state_t *state = malloc( sizeof(state_t) );

    state->bmp = bmp;
    state->bmp.width = WIDTH;
    state->bmp.height = HEIGHT;
    state->bmp.data = calloc(state->bmp.width * state->bmp.height, sizeof(color_bgr_t));
    state->theta = 0;
    state->time_step = 0;
    state->x = WIDTH / 2;
    state->y = HEIGHT / 2;

    image_server_start(port);
    //update_graphics(state);
    print_points(state);

/*     FILE *f = fopen("my_image.bmp", "wb");
    fwrite(state->image_data, state->image_size, 1, f);
    fclose(f);   */  

    for (int i = 0; i < time_steps; i++) {
        state->time_step = i + 1;
        update_movement(state);
        update_collisions(state);
        print_points(state);

        if (!fast) {
            update_graphics(state);
            int seconds = 0;
            long nanoseconds = 40 * 1000 * 1000;
            struct timespec interval = { seconds, nanoseconds };
            nanosleep(&interval, NULL); 
        }
    }
 
    if (fast) {
        update_graphics(state);
        sleep(1);
    } 

    free(state->image_data);
    free(state->bmp.data);
    free(state); 

    return 0;
}

/* For ./braitenberg 0 1 | sort
Incorrect sorted output on stdout. Use the following diff (limited to 10 lines) to help:
We expect to see: [0: After rounding: 307.00000 230.00000
0: After rounding: 307.00000 249.00000
0: After rounding: 333.00000 239.00000
0: Before rounding: 306.66667 230.00000
0: Before rounding: 306.66667 250.00000
0: Before rounding: 333.33333 240.00000] */