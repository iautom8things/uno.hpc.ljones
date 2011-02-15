#include <stdio.h>
#include <stdlib.h>


typedef struct{
	int x;
	int y;
	int z;
	int myCube;
} part;

typedef struct{
	part part[1000];
	int lastPart;
	double energy;
} cube;

unsigned char inBounds(int row, int column, int height);
int adjacents(int*);
int belongs_to_cube(int x, int y, int z);