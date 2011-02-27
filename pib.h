#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUMBER_OF_PARTICLES 10000 //number of particles in the system
#define SIZE 5 // This is the number of cubes on a axis of the box (5 = a 5x5x5 cube)
#define LENGTH_OF_CUBE 10 //length of a cube in angstroms
#define TOTAL_NUMBER_OF_CUBES SIZE*SIZE*SIZE //5x5x5 box = 125 cubes
#define MAX_NUMBER_OF_ADJACENT_CUBES 26

typedef struct{
	double x; //x coordinate loaction of the particle
	double y; //y coordinate loaction of the particle
	double z; //z coordinate loaction of the particle
	int myCube; //the cube that this particle is contained in
} particle;

typedef struct{
	particle *particles; //a dynamic array of particles for this cube
	int number_of_particles; // The number of particles in the cube
	long double energy; //the total energy of the cube
} cube;


unsigned char inBounds(int row, int column, int height);
int adjacents(int* adjacent_index_array, int index);
int belongs_to_cube(int x, int y, int z);
void addToCube(cube * a_cube, particle a_particle);
double distance(particle *a, particle *b);
long double calculate_pair_energy(double distance);
void calculate_cube_energy(cube *cubes, int cube_index);
int positiveVector(particle a, particle b);
particle* getParticles(int * neighbors, int num_neighbors, cube * cubes, int * total_particles);
