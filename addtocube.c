#include "pib.h"

/**
 * Adds the given particle to the given cube.
 * This function will grow the array of particles as needed
 *
 * @param aCube
 * @param aParticle
 */
void addToCube(cube * aCube, particle * aParticle){

	//If the array of particles is empty then create one particle location
	if((*aCube).lastParticle == -1)
		(*aCube).particles = (particle *)malloc(sizeof(particle));
        //if there are particles then add one more location
        else
		(*aCube).particles = (particle *)realloc((*aCube).particles , sizeof(particle)*((*aCube).lastParticle + 1 + 1));

        //increment the lastParticle position
        (*aCube).lastParticle += 1;
        //add the given particle to the array
	(*aCube).particles[(*aCube).lastParticle] = *aParticle;
}
