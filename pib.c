#include "pib.h"
#include <time.h>

/**
 * This main is used to test the progress of the solution to the
 * Lennard-Jones Problem.
 *
 * Code is being worked on by Manuel Zubieta, Daniel Ward, and Aaron Maus
 */

/**
 * Still need to perturb a particle and calulate the energy difference
 */

int main(int argc, char** argv){

    int i, j; // iterators
    srand(time(NULL)); // seed the random number generator
    int random_num = rand(); //get a random number

    cube cubes[TOTAL_NUMBER_OF_CUBES]; // allocate an array for our cubes

    //initalize the cubes in the array
    for(i=0;i<TOTAL_NUMBER_OF_CUBES;i++){
        cubes[i].number_of_particles = 0;
        cubes[i].energy = 0;
        cubes[i].particles = (particle *)malloc(0);
    }

    particle temp_particle;
    double x,y,z; //our temp variables for the x,y,and z coords

    /*
     * generate the particles and add then to the cube
     * that contains the x,y,z coordinates of the
     * ith particle
     */
    for(i=0;i<NUMBER_OF_PARTICLES;i++){
        random_num = rand();
        x = SIZE*LENGTH_OF_CUBE*((random_num)/(RAND_MAX+1.0));
        temp_particle.x = x;

        random_num = rand();
        y = SIZE*LENGTH_OF_CUBE*((random_num)/(RAND_MAX+1.0));
        temp_particle.y = y;

        random_num = rand();
        z = SIZE*LENGTH_OF_CUBE*((random_num)/(RAND_MAX+1.0));
        temp_particle.z = z;

        temp_particle.myCube = belongs_to_cube((int) x/10,(int) y/10,(int) z/10);

        addToCube(&cubes[temp_particle.myCube], temp_particle);
    }

    /*
     * calculate the energy of the ith cube and print some information
     * about the cube.
     */
    for(i=0; i< TOTAL_NUMBER_OF_CUBES; i++){

        calculate_cube_energy(cubes, i);
		//print the cube information
        printf("Cube %d:\n" , i);
        printf("Energy: %Lf\n" , cubes[i].energy);
        printf("Particles: %d\n\n", cubes[i].number_of_particles);
    }

    //print total energy of the system by summing the energy of the cubes
    long double total_energy = 0;
    for(i = 0; i < TOTAL_NUMBER_OF_CUBES; i++)
            total_energy += cubes[i].energy;

    printf("Total energy: %Lf \n\n", total_energy );
}

