#include "pib.h"
#include <time.h>
/*
Accomplishments so far.
Defined the structs to represent particles and cubes
randomply generated 1000 particles and assigned them
to an array of particle structures. The particle coordinates
are currently defined by integers because that is how the struct
was defined on the whiteboard.
7 Feb 2011, 8PM
--mau5*/

int main(int argc, char** argv){
	particle particles[NUMBER_OF_PARTICLES];
	int i; // an iterator
	srand(time(NULL)); // seed the random number generator
	int random_num = rand();
	
	cube cubes[TOTAL_NUMBER_OF_CUBES]; // allocate an array of our cubes

    for(i=0;i<TOTAL_NUMBER_OF_CUBES;i++){
        cubes[i].number_of_particles = 0;
        cubes[i].energy = 0;
    }

	particle temp_particle;
	double x,y,z; //our temp variables for the x,y,and z coords
	//generate n particles
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
        
        // Debugging print line
        //printf("(%f, %f, %f) -> %d\n", temp_particle.x, temp_particle.y, temp_particle.z, temp_particle.myCube);

		particles[i] = temp_particle;
        
        addToCube(&cubes[temp_particle.myCube], temp_particle);
	}
	
	/*
	   Debugging print of all information.
	*/
	/*
	for(i=0; i < TOTAL_NUMBER_OF_CUBES; i++){
        printf("Cube %d:\nNumber of particles: %d\n",i,cubes[i].number_of_particles);
        int j;
        for (j=0 ; j < cubes[i].number_of_particles; j++){
            printf("x: %f y: %f z: %f\n", cubes[i].particles[j].x, cubes[i].particles[j].y, cubes[i].particles[j].z);
        }
	}*/
	/*
	   test distance
	   temp_particle = cubes[124].particles[0];
    particle *other_particle = &cubes[124].particles[cubes[124].number_of_particles-1];
    double dist = distance(&temp_particle,other_particle);
    printf("A - \tx:%f\n\ty:%f\n\tz:%f\n\nB - \tx:%f\n\ty:%f\n\tz:%f\n\nDistance: %f\n",temp_particle.x, temp_particle.y, temp_particle.z,other_particle->x, other_particle->y, other_particle->z, dist);
    
    /*
        test energy
    */
    
   // printf("Pair energy: %f\n", calculate_pair_energy(dist));
    
    for(i=0; i< TOTAL_NUMBER_OF_CUBES; i++){

        calculate_cube_energy(cubes, i);
		//print the cube information
        printf("Cube %d:\nEnergy: %f \nParticles: %d \nNumber of Adj Particles: \n\n", i, cubes[i].energy, cubes[i].number_of_particles);
    }

	//print total energy of the system
	double total_energy = 0;
	for(i = 0; i < TOTAL_NUMBER_OF_CUBES; i++)
		total_energy += cubes[i].energy;

	printf("Total energy: %f \n\n", total_energy );

	double actual_energy = 0;

	int j;
    for (i = 0; i < NUMBER_OF_PARTICLES-1; i++){
        for (j = i+1; j < NUMBER_OF_PARTICLES; j++){

			//if(positiveVector( particles[i] , particles[j]))
            	actual_energy += calculate_pair_energy(distance(&particles[i], &particles[j]));
        }
    }

	printf("Actual energy: %f \n\n", actual_energy );
	
}

