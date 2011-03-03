#include "pib.h"


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

    int i; // iterators
    srand((unsigned)time(0)); // seed the random number generator
    double random_num = rand(); //get a random number


	//double energies[NUMBER_OF_TRIALS];//store accepted energies
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
        //printf("Cube %d:\n" , i);
        //printf("Energy: %f\n" , cubes[i].energy);
        //printf("Particles: %d\n\n", cubes[i].number_of_particles);
    }


	//start the simualtion
	printf("Starting the simulation with %d\n", NUMBER_OF_TRIALS);

	i = 0;//reset the iterator to use the while loop
	int failures = 0;

	FILE * file = fopen("data.csv","w");

	while(i < NUMBER_OF_TRIALS)
	{
		long double old_energy = system_energy(cubes);
		
		if(perturb(cubes) == 1)
		{
			fprintf(file,"Energy\t%4d\t%Lf\n",i,old_energy);			
			i++;
			
			// Fancy-schmancy progress bar :)
            int twentieth = NUMBER_OF_TRIALS/5;
            printf("\r%7d | %3d%% [",i, (i/twentieth)*5);
            int j;
            for (j=0;j<i/twentieth;j++){
                printf("--");
            }
            for (j=0;j<(20-i/twentieth);j++){
                printf("  ");
            }
            printf("] 100%%");
            fflush(0);

		}
		else
			failures++;
	}

	fclose(file);
	//for(i = 0; i < NUMBER_OF_TRIALS; i++)
	//  printf("energy %3d: %f\n" , i ,energies[i]);

	printf("\n");
	printf("Successes: %d\n", i);
	printf("Failures: %d\n" , failures);

	clean(cubes);

	return 1;
}

void clean(cube * cubes)
{
	int i;

	for(i = 0; i < TOTAL_NUMBER_OF_CUBES; i++)
		free(cubes[i].particles);
}

