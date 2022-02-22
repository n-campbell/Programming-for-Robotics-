#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>
#include <unistd.h>
#include <string.h>

#define WIDTH 640
#define HEIGHT 480


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
#define BLOCK_SIZE 40
#define MAP_W (WIDTH / BLOCK_SIZE)
#define MAP_H (HEIGHT / BLOCK_SIZE)


int main(void) {
    int x;
    int y;
    for ( int i = 0; i < MAP_H; i++ ) {
        for ( int j = 0; j < MAP_W; j++ )
            if ( MAP[ i * MAP_W + i ] == 'X' ) {
                x = 40 * i + ( 20 - 1 );
                y = 40 * j + ( 20 - 1 );
                printf("(i,j) = (%d,%d) \n",i,j);
                printf("(x,y) = (%d,%d) \n",x,y);
                
            }
    }
	return 0; 
}