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
    for (i = 0; i < n ; i++)
    {
        // compare it with all the other particles in this cube w/o repeats
        for (j = i; j < n; j++)
        {
            if(distance(c->particles[i], c->particles[j]) != 0)
                energy_sum += calculate_pair_energy(distance(c->particles[i], c->particles[j]));
        }


    }

    for(i = 0; i < n ; i++)
    {
        //compare the ith particle in this cube to the particles in the adjacent cubes
        for (j = 0; j < number_of_adjacent_particles; j++)
        {
            //to avoid extra energy, particles outside of this cube have half the energy
            energy_sum += 0.5 * calculate_pair_energy(distance(c->particles[i], adjacent_particles[j]));
        }
    }

    //free the allocated array
    free(adjacent_particles);
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

    //if distance s less than the minimum accepted threshold
    if(distance < MIN_DISTANCE)
        return 25247348603631.2;
        

    //if >+=to 10 angtroms the, calulate the enregy of the pair
    return (4*epsilon)*(pow((sigma/distance), 12) - pow((sigma/distance),6));
}//end calculate_pair_energy

/**
 * Compare to system energues and return the probability of acceptance
 *
 * @param energy_a Energy of the old state
 * @param energy_b Energy of the new state
 * @param temperature The temperature of the system
 * @return The probablity fo acceptance of the new energy
 * @author Daniel Ward
 */
double compare_energies(long double delta)
{
    long double boltzmann = 1.38;// * pow(10,-23);

    if(delta <= 0)
        return 1.0;
    else
        return exp(-1 * (delta)/(boltzmann*TEMPERATURE));
}//end compare_energies

/**
 * Combines all the enrgies in the given cubes
 * @param cubes The cubes that you want to sum up the enregies from
 * @return The total energy of the given cubes
 * @author Daniel Ward
 */
long double system_energy(cube * cubes)
{
    int i;
    long double energy = 0.0;

    for(i = 0; i < TOTAL_NUMBER_OF_CUBES; i++)
        energy += cubes[i].energy;

    return energy;
}//end system_energy

/**
 * Caluclate the energy of the cubes in the given list of cube numbers
 * @param cubes The array of cubes
 * @param cube_numbers THe array of cube numbers that you want to calculate
 * @param length The number of cubes that you are going to calculate
 * @return The sum of the enrgey of the cubes
 * @author Daniel Ward
 */
long double calculate_cube_list_energy(cube* cubes, int * cube_numbers, int length){

    int i;
    long double energy = 0.0;

    for(i = 0; i < length; i++)
            energy += cubes[cube_numbers[i]].energy;

    return energy;
}//end calculate_cube_list_energy

