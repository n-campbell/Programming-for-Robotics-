#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>
#include <unistd.h>
#include <string.h>

#include "image_server.h"
#include "graphics.h"
#include "collision.h"

#define WIDTH 640
#define HEIGHT 480

#define BLOCK_SIZE 40
#define MAP "XXXXXXXXXXXXXXXX" \
            "X              X" \
            "X  XXXX   XXX  X" \
            "X   XX      X  X" \
            "X       XXX    X" \
            "XXXXXX         X" \
            "X         XXXXXX" \
            "X    XXX       X" \
            "X  XX     XX   X" \
            "X   X    XX    X" \
            "X      XXX     X" \
            "XXXXXXXXXXXXXXXX"
#define MAP_W (WIDTH / BLOCK_SIZE)
#define MAP_H (HEIGHT / BLOCK_SIZE)

#define MAX_VEL 12

#define ROBOT_SIZE 20
#define ROBOT_BASE 80

#define MAX_SEARCH_DEPTH 4
#define N_SEARCH_ACTIONS 4

// we use rand() % RUNNER_RAND_TOTAL
// to decide on an action for the runner
// 1 and 2 are actual actions, everything else is no-action
#define RUNNER_RAND_TOTAL 20

#define TIME_DELAY_MS 40

#define M_PI 3.14159265358979323846

typedef struct agent {
    bool is_runner;
    double x;
    double y;
    double theta;
    double vel;
    double ang_vel;
} agent_t;

typedef struct state {
    int time_step;
    bitmap_t bmp;
    size_t image_size;
    uint8_t *image_data;

    bool runner_caught;

    agent_t runner;
    agent_t chaser;
} state_t;

typedef struct search_node {
    int depth;
    agent_t runner;
    agent_t chaser;
} search_node_t;

state_t *state_copy;
bool debug = false;


void get_block_vertices(vector_xy_t *block_points, xy_t center_coords) {
    block_points = gx_rect(BLOCK_SIZE, BLOCK_SIZE, block_points);
    block_points = gx_trans(center_coords.x, center_coords.y, block_points);
}

 //get coordinates of the robot
void get_vertices_robot(agent_t *agent, vector_xy_t *rpoints){
    double dim = 20;
    rpoints = gx_robot(dim, rpoints);
    rpoints = gx_rot(agent->theta, rpoints);
    rpoints = gx_trans(agent->x, agent->y, rpoints);
}

//get coordinates of the wall block
void get_vertices_block(int x, int y, vector_xy_t *bpoints){
    bpoints = gx_rect(BLOCK_SIZE, BLOCK_SIZE, bpoints);
    bpoints = gx_trans(x, y, bpoints);
}

void update_map_collisions(agent_t *agent) {

    vector_xy_t *robot = vector_xy_create();
    robot = gx_robot(ROBOT_SIZE,robot);
    robot = gx_rot(agent->theta, robot);
    robot = gx_trans(agent->x, agent->y, robot);

    bool any_collision = false;
    bool had_collision = true;

    // create robot polygon
    polygon_t pg_robot = {0};
    pg_robot.n_points = robot->size;

    for (int i = 0; i < robot->size; i++) {
        pg_robot.xs[i] = robot->data[i].x;
        pg_robot.ys[i] = robot->data[i].y;
    }
    
    vector_xy_destroy(robot);
    
    // create block polygon
    polygon_t pg_block = {0};
    xy_t block_center;

    while (had_collision) {
        had_collision = false;

        // find out where the robot is in the block map
        int robot_block_x = (agent->x - 20) / 40;
        int robot_block_y = (agent->y - 20) / 40;
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {

                // assign vertices for block polygon ( in the block map units )
                int block_x = robot_block_x + dx;
                int block_y = robot_block_y + dy;

                if ( MAP[ block_y * MAP_W + block_x ] == 'X' ) {    

                    // convert block points back to bitmap units
                    block_center.x = block_x * 40 + 20;
                    block_center.y = block_y * 40 + 20;
                    vector_xy_t *block_vertices = vector_xy_create();
                    get_block_vertices(block_vertices, block_center);
                    pg_block.n_points = block_vertices->size;

                    for (int j=0; j < block_vertices->size; j++) {
                        pg_block.xs[j] = block_vertices->data[j].x;
                        pg_block.ys[j] = block_vertices->data[j].y;
                    }   

                    vector_xy_destroy(block_vertices);

                    // if robot collides with block
                    if ( check_collides(&pg_robot, &pg_block) ) {
                        had_collision = true;
                        any_collision = true;

                        // create vector from robot center to block 
                        xy_t dir_vec;
                        double dir_vec_mag = sqrt( pow(block_center.x - agent->x , 2) + pow(block_center.y - agent->y, 2) );
                        dir_vec.x = ( block_center.x - agent->x ) / dir_vec_mag; // pointing from robot to wall
                        dir_vec.y = - ( block_center.y - agent->y ) / dir_vec_mag;
                        
                        agent->x -= 0.5 * dir_vec.x;
                        agent->y += 0.5 * dir_vec.y;

                        //update robot polygon
                        vector_xy_t *robot_update  = vector_xy_create();
                        robot_update  = gx_robot(ROBOT_SIZE,robot_update);
                        robot_update  = gx_rot(agent->theta, robot_update);
                        robot_update  = gx_trans(agent->x, agent->y, robot_update);

                        for (int i = 0; i < robot_update->size; i++) {
                            pg_robot.xs[i] = robot_update->data[i].x;
                            pg_robot.ys[i] = robot_update->data[i].y;
                        }

                        vector_xy_destroy(robot_update);
                    }
                }   
            }
        }
    }

    if (any_collision) {
        agent->vel *= 0.25;
    }

}  


bool agents_collide(agent_t *agent1, agent_t *agent2) {

    // agent one
    vector_xy_t * robot1 = vector_xy_create();
    robot1 = gx_robot(ROBOT_SIZE,robot1);
    robot1 = gx_rot(agent1->theta, robot1);
    robot1 = gx_trans(agent1->x, agent1->y, robot1);
    
    polygon_t pg_robot1 = {0};
    pg_robot1.n_points = robot1->size;

    for (int i = 0; i < robot1->size; i++) {
        pg_robot1.xs[i] = robot1->data[i].x;
        pg_robot1.ys[i] = robot1->data[i].y;
    }

    // agent two
    vector_xy_t * robot2 = vector_xy_create();
    robot2 = gx_robot(ROBOT_SIZE,robot2);
    robot2 = gx_rot(agent2->theta, robot2);
    robot2 = gx_trans(agent2->x, agent2->y, robot2);

    polygon_t pg_robot2 = {0};
    pg_robot2.n_points = robot2->size;

    for (int i = 0; i < robot2->size; i++) {
        pg_robot2.xs[i] = robot2->data[i].x;
        pg_robot2.ys[i] = robot2->data[i].y;
    }

    vector_xy_destroy(robot1);
    vector_xy_destroy(robot2);

    // check collision
    if ( check_collides(&pg_robot1,&pg_robot2) ) {
        return true;
    }
    else {
        return false;
    }

} 
 
void update_collisions(state_t *state) {
    update_map_collisions(&state->chaser);
    update_map_collisions(&state->runner);

    if (agents_collide(&state->runner, &state->chaser)) {
        fprintf(stderr, "Runner caught on step %d\n", state->time_step);
        state->runner_caught = true;
    }
} 




void update_agent_movement(agent_t *agent, int action) {
    switch (action) {
    case 0:
        break;
    case 1:
        // increase velocity by 2 (capped at 12)
       if ( (agent->vel + 2) <= 12 ) {
            agent->vel += 2;
        }
        else if ( (agent->vel + 2) > 12 ) {
            agent->vel = 12;
        }  
        break;
    case 2:
        // increase angular velocity by PI/16 CCW
        agent->ang_vel -= M_PI/16;
        break;
    case 3:
        // (chaser only) increase angular velocity by PI/16 CW
        if (!(agent->is_runner) ) {
            agent->ang_vel += M_PI/16;
        }
        break;
    default:
        // do nothing
        break;
    }

    agent->theta -= agent->ang_vel;
    agent->ang_vel *= 0.8;

    agent->x += agent->vel * cos(agent->theta);
    agent->y -= agent->vel * sin(agent->theta);
}  



void update_graphics(state_t *state) {
    bitmap_t *b = &state->bmp;

    color_bgr_t black = {0, 0, 0};
    color_bgr_t white = {255, 255, 255};
    color_bgr_t red = {0, 0, 255};
    color_bgr_t green = {0, 255, 0};
    color_bgr_t yellow = {0, 255, 255};

    // draw background color
    gx_clear(b, white);

    // draw black blocks 
    int x;
    int y;
    for ( int i = 0; i < MAP_W; i++ ) {
        for ( int j = 0; j < MAP_H; j++ )
            if ( MAP[ j * MAP_W + i ] == 'X' ) {
                x = 40 * i + 20;
                y = 40 * j + 20;
                vector_xy_t * block_points = vector_xy_create();
                gx_rect(BLOCK_SIZE, BLOCK_SIZE, block_points);
                gx_trans(x, y, block_points);
                gx_fill(b, black, block_points);
                vector_xy_destroy(block_points);
            }
    }

    // draw chaser robot
    vector_xy_t * chaser_robot = vector_xy_create();
    chaser_robot = gx_robot(20, chaser_robot);
    chaser_robot = gx_rot(state->chaser.theta, chaser_robot);
    chaser_robot = gx_trans(state->chaser.x,state->chaser.y, chaser_robot);
    gx_fill(b, red, chaser_robot);

    // draw runner robot
    vector_xy_t * runner_robot = vector_xy_create();
    runner_robot = gx_robot(20, runner_robot);
    runner_robot = gx_rot(state->runner.theta, runner_robot);
    runner_robot = gx_trans(state->runner.x,state->runner.y, runner_robot);
    gx_fill(b, green, runner_robot);
    
    // serialize bit map
    bmp_serialize(&state->bmp, state->image_data);
    image_server_set_data(state->image_size, state->image_data);

    vector_xy_destroy(chaser_robot);
    vector_xy_destroy(runner_robot);
}  

void simulate_timestep(search_node_t *node, int chaser_action) {
    update_agent_movement(&node->chaser, chaser_action);
    update_map_collisions(&node->chaser);
} 


double search_actions(search_node_t node, int *chosen_action) {

    if ( agents_collide( &node.runner, &node.chaser) ) {
        return 0;
    }
    if (node.depth >= MAX_SEARCH_DEPTH) {
        return sqrt( pow(node.runner.x - node.chaser.x, 2) + pow(node.runner.y - node.chaser.y, 2) );
    }

    double best_score = DBL_MAX;

    for (int i = 0; i < N_SEARCH_ACTIONS; i++) {

        search_node_t action_node = {
            .depth = node.depth + 1,
            .runner = node.runner,
            .chaser = node.chaser,
        };

        simulate_timestep(&action_node, i);

        int n_extra_steps = 3;
        for (int t = 0; t < n_extra_steps; t++) {
            if (agents_collide(&action_node.runner, &action_node.chaser)) {
                break;
            }
            simulate_timestep(&action_node, 0);
        }

        double score = search_actions(action_node, NULL);
        score += 300 / fmin(2, action_node.chaser.vel);

        if (score < best_score) {
            if ( chosen_action ) {
                *chosen_action = i;
            }
            best_score = score;
        }
    }

    return best_score;
}  

int choose_best_action(state_t *state) {
    search_node_t search_node = {
        .depth = 0,
        .runner = state->runner,
        .chaser = state->chaser};

    int best_action = 0;
    search_actions(search_node, &best_action);
    return best_action;
} 

void update_movement(state_t *state) {
    int chase_action = choose_best_action(state);
    update_agent_movement(&state->chaser, chase_action);

    int run_action = rand() % RUNNER_RAND_TOTAL;
    if (run_action != 1 && run_action != 2) {
        run_action = 0;
    }

    update_agent_movement(&state->runner, run_action);

    printf("%d %d\n", run_action, chase_action);
}  

int main(int argc, char **argv) {

    if (argc < 4 || argc > 5) {
        fprintf(stderr, "usage: %s <time steps> <fast=0|1|2> <initial runner index>\n",
                argv[0]);
        return 1;
    }

    int time_steps = atoi(argv[1]);
    int fast = atoi(argv[2]);
    int initial_runner_idx = atoi(argv[3]);

    if (MAP[initial_runner_idx] != ' ') {
        fprintf(stderr, "Cannot start runner in a wall\n");
        return 1;
    } 

    state_t state_var = {0};
    state_t *state = &state_var;
    state_copy = state;

    state->runner.is_runner = true;
    int map_idx_x = initial_runner_idx % MAP_W;
    int map_idx_y = initial_runner_idx / MAP_W;
    state->runner.x = (map_idx_x + 0.5) * BLOCK_SIZE;
    state->runner.y = (map_idx_y + 0.5) * BLOCK_SIZE;
    state->runner.theta = 0;

    state->chaser.x = WIDTH / 2.0;
    state->chaser.y = HEIGHT / 2.0;
    state->chaser.theta = 0;

    state->bmp.width = WIDTH;
    state->bmp.height = HEIGHT;
    state->bmp.data = calloc(WIDTH * HEIGHT, sizeof(color_bgr_t));

    state->image_size = bmp_calculate_size(&state->bmp);
    state->image_data = malloc(state->image_size);

    if (fast != 2) {
        image_server_start("8000");
    }

    for (int i = 0; i < time_steps; i++) {
        state->time_step = i;

        xy_t chaser_before;
        chaser_before.x = state->chaser.x;
        chaser_before.y = state->chaser.y;

        xy_t runner_before;
        runner_before.x = state->runner.x;
        runner_before.y = state->runner.y;

        update_movement(state);
        update_collisions(state);

        if (!fast) {
            update_graphics(state);
            struct timespec interval = {0, TIME_DELAY_MS * 1000 * 1000};
            nanosleep(&interval, NULL);
        }

        if (state->runner_caught) {
            break;
        }
    }
    if (fast == 1) {
        update_graphics(state);
        struct timespec interval = {0, 100 * 1000 * 1000};
        nanosleep(&interval, NULL);
    }

    free(state->image_data);
    free(state->bmp.data);

    return 0;
}
