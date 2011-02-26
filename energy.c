#include "pib.h"

/**
* This should take the current cube and all the neighbor cubes 
* to calculate the energy given to the system.
*/
void calculate_cube_energy(cube *cubes, int cube_index){
    cube c = cubes[cube_index];
    int adjs[26]; // 26 is the max number of possible adjacent cubes
    int number_of_adjacents = adjacents(adjs, cube_index);
    int number_of_adjacent_particles;
    particle *adjacent_particles = getParticles(adjs, number_of_adjacents, cubes , &number_of_adjacent_particles);
    int n = c.number_of_particles, i, j;
    double energy_sum = 0;
    
    for (i = 0; i < n - 1; i++){ // for each particle in the cube
        for (j = i+1; j < n; j++){ // compare it with all the other particles in the cube w/o repeats
            energy_sum += calculate_pair_energy(distance(&c.particles[i], &c.particles[j]));
        }
        for (j = 0; j < number_of_adjacent_particles; i++){
            energy_sum += calculate_pair_energy(distance(&c.particles[i], &adjacent_particles[j]));
        }
    }
    c.energy = energy_sum;
}

double calculate_pair_energy(double distance){
    // This could very-well be right OR wrong...grabbed it from HPC's first attempt at this problem.
    if (distance > 10)
        return 0;
    return (4*.2)*(pow((4.0/distance), 12) - pow((4.0/distance),6));
}
