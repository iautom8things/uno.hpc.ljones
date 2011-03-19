#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <mpi.h>

#define NUMBER_OF_PARTICLES 1000 //number of particles in the system
#define SIZE 5 // This is the number of cubes on a axis of the box (5 = a 5x5x5 cube)
#define LENGTH_OF_CUBE 10 //length of a cube in angstroms
#define TOTAL_NUMBER_OF_CUBES SIZE*SIZE*SIZE //5x5x5 box = 125 cubes
#define MAX_NUMBER_OF_ADJACENTS 26
#define MIN_DISTANCE 0.3
int NUMBER_OF_TRIALS; //number of trials to run
int TEMPERATURE; //temperature of simulation in kelvin

int id, nprocs; // MPI

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

particle particle_array[NUMBER_OF_PARTICLES];
cube cubes[TOTAL_NUMBER_OF_CUBES];

//Found in pib.c
void clean();

//Found in adjacents.c
particle* get_particles_from_cubes(int * neighbors, int num_neighbors, cube * cubes, int * total_particles);
unsigned char in_bounds(int row, int column, int height);
int adjacents(int* adjacent_index_array, int index);
int belongs_to_cube(double x, double y, double z);
void remove_particle(cube * a_cube, particle a_particle);
long double delta_energy(int old_index, particle new_particle);

//Found in addtocube.c
void addToCube(cube * a_cube, particle a_particle);

//Found in distance.c
double distance(particle a, particle b);

//Found in energy.c
long double system_energy();
long double calculate_pair_energy(double distance);
long double calculate_cube_list_energy(int * cube_numbers, int length);
void calculate_cube_energy(int cube_index);
double compare_energies(long double delta);

//Found in parallel.c
void setup_tree (int max_buff_size, int childrens_max_buff_size, double *previous_state, double *current_peturbing, double *accepted_state, double *rejected_state);
void update_state(double *previous_state, int max_buff_size);
void perturb(double *current_perturbing, long double * result);
