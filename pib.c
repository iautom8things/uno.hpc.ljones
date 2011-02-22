#include "pib.h"
#include <time.h>
//Accomplishments so far.
//Defined the structs to represent particles and cubes
//randomply generated 1000 particles and assigned them
//to an array of particle structures. The particle coordinates
//are currently defined by integers because that is how the struct
//was defined on the whiteboard.
// 7 Feb 2011, 8PM
// --mau5



int main(int argc, char** argv){
	int i; //an iterator
	//seed the random number generator
	srand(time(NULL));
	int random_num = rand();
	//allocate an array of our cubes
	cube cubes[NUM_CUBES];
    cube c;
    for(i=0;i<NUM_CUBES;i++){
        cubes[i].lastPart = -1;
        cubes[i].energy = 0;
    }

	//allocate a 2-d array to store our particles
	part particles[NUM_PARTICLES];
	part particle;

	//our temp variables for the x,y,and z coords
	//to create n particles
	float x,y,z;
	
	//generate n particles
	for(i=0;i<NUM_PARTICLES;i++){
	    random_num = rand();
	    x = 50*((random_num)/(RAND_MAX+1.0));
	    particle.x = x;

	    random_num = rand();
	    y = 50*((random_num)/(RAND_MAX+1.0));
	    particle.y = y;

	    random_num = rand();
	    z = 50*((random_num)/(RAND_MAX+1.0));
	    particle.z = z;
	    
        particle.myCube = belongs_to_cube(x/10,y/10,z/10);
        //notify cube
        //cube *c = &cubes[particle.myCube];
        //c->part[c->lastPart++] = particle;//add particle to cube's list of particles and increase lastPart
	    particles[i] = particle;	
        addToCube(&cubes[particle.myCube], &particle);
	}

	/*/print out our particles
	for(i=0;i<NUM_PARTICLES;i++)
		printf("%d, %d, %d :: %d\n",particles[i].x,
			particles[i].y,particles[i].z, particles[i].myCube);*/
	for(i=0; i < NUM_CUBES; i++){
        printf("Cube %d:\nNumber of particles: %d\n",i,cubes[i].lastPart);
        int j;
        for (j=0 ; j < cubes[i].lastPart; j++){
            printf("x: %d y: %d z: %d\n", cubes[i].part[j].x, cubes[i].part[j].y, cubes[i].part[j].z);
        }
	}
}

