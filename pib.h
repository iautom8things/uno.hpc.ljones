#include <stdio.h>
#include <stdlib.h>

#define NUM_PARTICLES 10
#define SIZE 2 // This is the number of cubes on a axis of the box (5 = a 5x5x5 cube)
#define NUM_CUBES SIZE*SIZE*SIZE //5x5x5 box = 125 cubes
typedef struct{
	int x;
	int y;
	int z;
	int myCube;
} part;

typedef struct{
	part *part;
	int lastPart;
	double energy;
} cube;

unsigned char inBounds(int row, int column, int height);
int adjacents(int*);
int belongs_to_cube(int x, int y, int z);
void addToCube(cube * aCube, part * aParticle);