#include "pib.h"
#include <time.h>


void copy_system(cube * temp_system,cube * cubes)
{
	int i,j;
	for(i = 0; i < TOTAL_NUMBER_OF_CUBES; i++){

		temp_system[i].number_of_particles = cubes[i].number_of_particles;
		temp_system[i].energy = cubes[i].energy;

		for (j = 0; j < temp_system[i].number_of_particles; j++){

			cubes[i].particles[j].x = cubes[i].particles[j].x;
			cubes[i].particles[j].y = cubes[i].particles[j].y;
			cubes[i].particles[j].z = cubes[i].particles[j].z;
			cubes[i].particles[j].myCube = cubes[i].particles[j].myCube;
		}
	}
	
}

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


	long double energies[NUMBER_OF_TRIALS];//store accepted energies
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
    long double total_energy = system_energy(cubes);

    printf("Total energy: %Lf \n\n", total_energy );

	//start the simualtion
	printf("Starting the simulation with %d", NUMBER_OF_TRIALS);

	i = 0;//reset the iterator to use the while loop
	long double temperature = 100.0;//in Kelvins

	while(i < NUMBER_OF_TRIALS)
	{
		long double old_energy = system_energy(cubes);
		cube temp_system[TOTAL_NUMBER_OF_CUBES]; 

		copy_system(temp_system,cubes);
		
		//move particle in the temp_system
		//get the two changed cubes in the temp_system
		//calcualte the new energy of the cubes affected in the temp_system 

		long double new_energy = system_energy(temp_system);

		if(compare_energies(old_energy,new_energy,temperature) > ACCEPTABLE_PROBABILITY)
		{
			//keep the accepted energies
			energies[i] = old_energy;
			//update the old sytem with the new system
			copy_system(cubes,temp_system);
			i++;
		}
		
	}
}

