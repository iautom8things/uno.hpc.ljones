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

    int i; //an iterator for the 'for' loops

    //seed the random number generator
    srand(time(NULL));
    int random_num = rand();
    
    //allocate an array of our cubes
    cube cubes[NUM_CUBES];

    //initialize the cubes
    for(i=0;i<NUM_CUBES;i++){
        cubes[i].lastParticle = -1;
        cubes[i].energy = 0;
    }

    //allocate a 2-d array to store our particles
    //particle particles[NUM_PARTICLES];
    particle tempParticle;

    //our temp variables for the x,y,and z coords
    //to create n particles
    float x,y,z;


    //generate n particles
    for(i=0;i<NUM_PARTICLES;i++){
        random_num = rand();
        x = 50*((random_num)/(RAND_MAX+1.0));
        tempParticle.x = x;

        random_num = rand();
        y = 50*((random_num)/(RAND_MAX+1.0));
        tempParticle.y = y;

        random_num = rand();
        z = 50*((random_num)/(RAND_MAX+1.0));
        tempParticle.z = z;

        tempParticle.myCube = belongs_to_cube(x/10,y/10,z/10);

        addToCube(&cubes[tempParticle.myCube], &tempParticle);
    }

    for(i=0; i < NUM_CUBES; i++){
        printf("Cube %d:\nNumber of particles: %d\n",i,cubes[i].lastParticle+1);

        int j;

        for (j=0 ; j < cubes[i].lastParticle; j++){
            printf("x: %d y: %d z: %d\n", cubes[i].particles[j].x, cubes[i].particles[j].y, cubes[i].particles[j].z);
        }
    }
}

