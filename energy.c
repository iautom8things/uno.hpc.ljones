#include "pib.h"

/**
* This sshould take the current cube and all the neighbor cubes 
* to calcualte the energy given to the system.
*/
void calculate_cube_energy(cube *c){
    int n = c->number_of_particles, i, j;
    double energy_sum = 0;
    
    for (i = 0; i < n; i++){
        for (j = i; j < n; j++){

			if(positiveVector(&c->particles[i] , &c->particles[j]))
            	energy_sum += calculate_pair_energy(distance(&c->particles[i], &c->particles[j]));
        }
    }
    (*c).energy = energy_sum;
}

double calculate_pair_energy(double distance){
    // This could very-well be right OR wrong...grabbed it from HPC's first attempt at this problem.
    // Needless to say these magic numbers HAVE to @*#$ing go. 
    
    return (4*.2)*(pow((4.0/distance), 12) - pow((4.0/distance),6));
}
