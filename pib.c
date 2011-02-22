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
	int i;                          // an iterator
	srand(time(NULL));              // seed the random number generator
	int random_num = rand();
	
	cube cubes[TOTAL_NUMBER_OF_CUBES];          // allocate an array of our cubes

    for(i=0;i<TOTAL_NUMBER_OF_CUBES;i++){
        //cubes[i].part = (part *)malloc(sizeof(part));
        cubes[i].number_of_particles = 0;
        cubes[i].energy = 0;
    }

	part particle;
	float x,y,z; //our temp variables for the x,y,and z coords
	
	//generate n particles
	for(i=0;i<NUMBER_OF_PARTICLES;i++){
	    random_num = rand();
	    x = SIZE*LENGTH_OF_CUBE*((random_num)/(RAND_MAX+1.0));//FUCKING MAGIC NUMBERS
	    particle.x = x;

	    random_num = rand();
	    y = SIZE*LENGTH_OF_CUBE*((random_num)/(RAND_MAX+1.0));
	    particle.y = y;

	    random_num = rand();
	    z = SIZE*LENGTH_OF_CUBE*((random_num)/(RAND_MAX+1.0));
	    particle.z = z;
	    
        particle.myCube = belongs_to_cube(x/10,y/10,z/10);
        
        printf("(%d, %d, %d) -> %d\n", particle.x, particle.y, particle.z, particle.myCube);
        
        addToCube(&cubes[particle.myCube], particle);
	}

	for(i=0; i < TOTAL_NUMBER_OF_CUBES; i++){
        printf("Cube %d:\nNumber of particles: %d\n",i,cubes[i].number_of_particles);
        int j;
        for (j=0 ; j < cubes[i].number_of_particles; j++){
            printf("x: %d y: %d z: %d\n", cubes[i].part[j].x, cubes[i].part[j].y, cubes[i].part[j].z);
        }
	}
}

