#include "pib.h"


void copy_system(cube * temp_system,cube * cubes)
{
	int i,j;

	for(i = 0; i < TOTAL_NUMBER_OF_CUBES; i++){

		temp_system[i].number_of_particles = cubes[i].number_of_particles;
		temp_system[i].energy = cubes[i].energy;
		
		free(temp_system[i].particles);
		temp_system[i].particles = (particle *)malloc(sizeof(particle)*cubes[i].number_of_particles);

		for (j = 0; j < cubes[i].number_of_particles; j++){
			temp_system[i].particles[j].x = cubes[i].particles[j].x;
			temp_system[i].particles[j].x = cubes[i].particles[j].x;
			temp_system[i].particles[j].x = cubes[i].particles[j].x;
			temp_system[i].particles[j].myCube = cubes[i].particles[j].myCube;
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
    double random_num = rand(); //get a random number


	double energies[NUMBER_OF_TRIALS];//store accepted energies
    cube cubes[TOTAL_NUMBER_OF_CUBES]; // allocate an array for our cubes

    //initalize the cubes in the array
    for(i=0;i<TOTAL_NUMBER_OF_CUBES;i++){cubes[i].particles[j].x;
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
        //printf("Cube %d:\n" , i);
        //printf("Energy: %f\n" , cubes[i].energy);
        //printf("Particles: %d\n\n", cubes[i].number_of_particles);
    }

    //print total energy of the system by summing the energy of the cubes
    double total_energy = system_energy(cubes);

    //printf("Total energy: %f \n\n", total_energy );

	//start the simualtion
	printf("Starting the simulation with %d\n", NUMBER_OF_TRIALS);

	i = 0;//reset the iterator to use the while loop
	double temperature = 1000.0;//in Kelvins

	while(i < NUMBER_OF_TRIALS)
	{
		double old_energy = system_energy(cubes);
		cube temp_system[TOTAL_NUMBER_OF_CUBES]; 

		copy_system(temp_system,cubes);
		
		perturb(temp_system);

		double new_energy = system_energy(temp_system);

		double probability = compare_energies(old_energy,new_energy,temperature);
		//printf("%30.10f\n", old_energy);
		//printf("%30.10f\n", new_energy);

		if(probability > ACCEPTABLE_PROBABILITY)
		{

			printf("success: %d\n", i);
			//keep the accepted energies
			energies[i] = old_energy;
			//update the old sytem with the new system
			copy_system(cubes,temp_system);
			i++;
		}
		i++;
	}

	//for(i = 0; i < NUMBER_OF_TRIALS; i++)
	//	printf("%f" , energies[i]);
}

