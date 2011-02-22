#include <stdio.h>
#include <stdlib.h>

#define NUM_PARTICLES 1000
#define SIZE 5 // This is the number of cubes on a axis of the box (5 = a 5x5x5 cube)
#define NUM_CUBES SIZE*SIZE*SIZE //5x5x5 box = 125 cubes
typedef struct{
	int x;
	int y;
	int z;
	int myCube;
} part;

typedef struct{
	part part[NUM_PARTICLES];
	int lastPart;
	double energy;
} cube;

unsigned char inBounds(int row, int column, int height);
int adjacents(int*);
int belongs_to_cube(int x, int y, int z);