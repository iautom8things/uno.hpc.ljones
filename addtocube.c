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

    //expand a temporary array to hold the particles in a_cube plus a new one
    particle * temp = (particle *)malloc(sizeof(particle)* (a_cube->number_of_particles + 1));

    //copy the particles from a_cube to the new array
    int i;
    for(i = 0; i < a_cube->number_of_particles; i++)
        temp[i] = a_cube->particles[i];

    //free the old array
    free(a_cube->particles);

    //assign the new array to a_cube
    a_cube->particles = temp;

    //add the particle to the last place in the particle array of the given cube
    a_cube->particles[a_cube->number_of_particles] = a_particle;

    //increase number_of_particle in the given cube
    a_cube->number_of_particles++;
}
