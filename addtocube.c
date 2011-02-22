#include "pib.h"

/**
 * Adds the given particle to the given cube.
 * This function will grow the array of particles as needed
 *
 * @param aCube
 * @param aParticle
 */
void addToCube(cube * aCube, particle aParticle){  

	if(aCube->number_of_particles ==0)
		aCube->particles = (particle *)malloc(sizeof(particle));
	else		      
		aCube->particles = (particle *)realloc(aCube->particles , sizeof(particle)*aCube->number_of_particles + sizeof(particle));

	aCube->particles[aCube->number_of_particles] = aParticle;
	aCube->number_of_particles++;
}
