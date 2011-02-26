#include "pib.h"

void calculate_cube_energy(cube *c){
    int n = c->number_of_particles, i, j;
    double energy_sum = 0;
    
    for (i = 0; i < n-1; i++){
        for (j = i+1; j < n; j++){
            energy_sum += calculate_pair_energy(distance(&c->particles[i], &c->particles[j]));
        }
    }
    c->energy = energy_sum;
}

double calculate_pair_energy(double distance){
    // This could very-well be right OR wrong...grabbed it from HPC's first attempt at this problem.
    // Needless to say these magic numbers HAVE to @*#$ing go. 
    
    return 13421773*pow(distance,-6) - 3276.8*pow(distance,-3);
}