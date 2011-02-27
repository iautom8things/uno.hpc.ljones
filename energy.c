#include "pib.h"

/**
 * Calulates the energy contributed to the system by the cube at the
 * given index.
 *
 * @param cubes An array of cubes
 * @param cube_index The cube at the given index to calculate energy
 * @author Manuel Zubieta
 */
void calculate_cube_energy(cube *cubes, int cube_index){

    //a pointer the the cube that we need to calulate energy for
    cube * c = &cubes[cube_index];

    int adjacents_indices[MAX_NUMBER_OF_ADJACENTS];

    //get the number of adjacents and fill the adjacents array
    int number_of_adjacents = adjacents(adjacents_indices, cube_index);
    
    //get the numebr and an array of the adjacent particles
    int number_of_adjacent_particles;
    particle *adjacent_particles = get_particles_from_cubes(adjacents_indices, number_of_adjacents, cubes , &number_of_adjacent_particles);
    
    //the number of particles in this cube and some iterators
    int n = (*c).number_of_particles, i, j;
    
    //the sum of the energy for this cube
    long double energy_sum = 0;
    
    // for each particle in this cube
    for (i = 0; i < n - 1; i++)
    {
        // compare it with all the other particles in this cube w/o repeats
        for (j = i+1; j < n; j++)
        {
            energy_sum += calculate_pair_energy(distance(c->particles[i], c->particles[j]));
        }

        //compare the ith particle in this cube to the particles in the adjacent cubes
        for (j = 0; j < number_of_adjacent_particles; j++)
        {
            energy_sum += calculate_pair_energy(distance(c->particles[i], adjacent_particles[j]));
        }
    }

    //assign the energy to this cube
    (*c).energy = energy_sum;
}//end calculate_cube_energy

/**
 * This will calculate the energy from a pair of
 * particles using thier distance
 * 
 * @param distance The disatnce between a pair of particles
 * @return The energy of the pair of particels if the distance is >= to 10 Angstroms
 * @author Manuel Zubieta
 */
long double calculate_pair_energy(double distance){

    //these are compents of the energy function.
    //these are used to change the values later on
    double epsilon = .2;
    double sigma = 4.0;

    //if distance is < than 10 angtroms, then assume 0 energy
    if (distance > 10)
        return 0;

    //if >+=to 10 angtroms the, calulate the enregy of the pair
    return (4*epsilon)*(pow((sigma/distance), 12) - pow((sigma/distance),6));
}

// return the probablilty of acceptance
int compare_energies(long double energy_a, long double energy_b, long double temperature)
{
	long double boltzmann = 1.38 * pow(10,-23);

	if(energy_b <= energy_a)
		return 1;
	else
		return exp(-1 * (energy_b - energy_a)/(boltzmann*temp));
}
