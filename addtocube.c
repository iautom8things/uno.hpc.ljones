#include "pib.h"


/**
 * Adds the given particle to the given cube.
 * This function will grow the array of particles as needed
 *
 * @param a_cube The cube the the given particle will be added to
 * @param a_particle The particle to be added to the given cube
 * 
 * @author Daniel Ward
 */
void addToCube(cube * a_cube, particle a_particle){

    //expand the array of particles by one particle element
    a_cube->particles = (particle *)realloc(a_cube->particles , sizeof(particle)*a_cube->number_of_particles + sizeof(particle));

    //add the particle to the last place in the particle array of the given cube
    a_cube->particles[a_cube->number_of_particles] = a_particle;
    //increase number_of_particle in the given cube
    a_cube->number_of_particles++;
}
