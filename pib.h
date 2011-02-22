#include <stdio.h>
#include <stdlib.h>

#define NUMBER_OF_PARTICLES 1000
#define SIZE 5 // This is the number of cubes on a axis of the box (5 = a 5x5x5 cube)
#define LENGTH_OF_CUBE 10
#define TOTAL_NUMBER_OF_CUBES SIZE*SIZE*SIZE //5x5x5 box = 125 cubes
typedef struct{
	int x;
	int y;
	int z;
	int myCube;
} part;

typedef struct{
	int number_of_particles;
	double energy;
	part *part;
} cube;

unsigned char inBounds(int row, int column, int height);
int adjacents(int*);
int belongs_to_cube(int x, int y, int z);
void addToCube(cube * aCube, part aParticle);