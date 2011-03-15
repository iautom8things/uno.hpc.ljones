#include "pib.h"


/**
 *
 * @param cubes The set cubes that need to be perturb
 * @return 1 if the new state is acceptable, else 0
 * @author Daniel Ward
 */
int perturb(cube * cubes)
{
    int i; //iterator
	int index_of_random = (int)rand()%NUMBER_OF_PARTICLES;

	int adjacents_indices[MAX_NUMBER_OF_ADJACENTS];
    int number_of_adjacents;

	int old_cube_indices[MAX_NUMBER_OF_ADJACENTS];
    int number_of_old_cubes;
	particle * neighbor_particles_of_random;
	int number_of_neighbors;
	
	long double old_cube_energies[MAX_NUMBER_OF_ADJACENTS];

	long double old_energy;
	long double new_energy;
	
	long double energy_change_at_old = 0.0;
	long double energy_change_at_new = 0.0;
	long double energy_at_old_initial;
	long double energy_at_old_final;

	long double energy_at_new_initial;
	long double energy_at_new_final;
	
	//inital energy of the system
    old_energy = system_energy(cubes);

	particle random_particle = particle_array[index_of_random];

	//get old cube energies
    number_of_old_cubes = adjacents(old_cube_indices, random_particle.myCube);
	for(i = 0; i < number_of_old_cubes; i++)
		old_cube_energies[i] = cubes[old_cube_indices[i]].energy;		

	//get initial energy
	energy_at_old_initial = calculate_cube_list_energy(cubes,old_cube_indices, number_of_old_cubes);

	//change the energies of the old cubes
	neighbor_particles_of_random = get_particles_from_cubes(old_cube_indices, number_of_old_cubes, cubes, &number_of_neighbors);

	for(i = 0; i < number_of_neighbors; i++)
	{
		if(random_particle.myCube != neighbor_particles_of_random[i].myCube)
		{
			long double energy = calculate_pair_energy(distance(random_particle, neighbor_particles_of_random[i]));
			cubes[random_particle.myCube].energy -= energy;
			cubes[neighbor_particles_of_random[i].myCube].energy -= energy;
			energy_change_at_old += 2 * energy;
		}
		else{
			long double energy = calculate_pair_energy(distance(random_particle, neighbor_particles_of_random[i]));
			cubes[random_particle.myCube].energy -= energy;
			energy_change_at_old += energy;			
		}
	}

    //remove the particle
    remove_particle(&cubes[random_particle.myCube], random_particle);

	//get change energy
	energy_at_old_final = calculate_cube_list_energy(cubes,old_cube_indices, number_of_old_cubes); 

    //create a new particle to be added to a random_particle cube/////////////
    particle temp;
    
    temp.x = SIZE*LENGTH_OF_CUBE*((rand())/(RAND_MAX+1.0));
    temp.y = SIZE*LENGTH_OF_CUBE*((rand())/(RAND_MAX+1.0));
    temp.z = SIZE*LENGTH_OF_CUBE*((rand())/(RAND_MAX+1.0));
    temp.myCube = belongs_to_cube((int) temp.x/10,(int) temp.y/10,(int) temp.z/10);
    
    /////////////Done create a new particle//////////////////////////

	//get cube information at new location
	number_of_adjacents = adjacents(adjacents_indices, temp.myCube);

	//get the energy at the new loaction before altering it
	energy_at_new_initial = calculate_cube_list_energy(cubes,adjacents_indices, number_of_adjacents);

	//add the particle to the new cube
	addToCube(&cubes[temp.myCube], temp);
	
    //recalulate the energy at the new cube location
	int number_of_neighbors_at_new;
	int new_cube_indices[MAX_NUMBER_OF_ADJACENTS];
	int number_new_cubes;

	number_new_cubes = adjacents(new_cube_indices, temp.myCube);
    particle * neighbor_particles_at_new = get_particles_from_cubes(new_cube_indices, number_new_cubes, cubes, &number_of_neighbors_at_new);

	for(i = 0; i < number_of_neighbors_at_new; i++)
	{
		if(temp.myCube != neighbor_particles_at_new[i].myCube)
		{
			long double energy = calculate_pair_energy(distance(temp, neighbor_particles_at_new[i]));
			cubes[temp.myCube].energy -= energy;
			cubes[neighbor_particles_at_new[i].myCube].energy -= energy;
			energy_change_at_new += 2 * energy;
		}
		else{
			long double energy = calculate_pair_energy(distance(temp, neighbor_particles_at_new[i]));
			cubes[temp.myCube].energy -= energy;
			energy_change_at_new += energy;			
		}
	}

    //get the new energy of the new cube location
    energy_at_new_final = calculate_cube_list_energy(cubes,adjacents_indices, number_of_adjacents);

    //get the new energy of the system
    new_energy = old_energy + (energy_change_at_new - energy_change_at_old);

    //calulate the probability of acceptance
    double probability = compare_energies(old_energy,new_energy,TEMPERATURE);

	free(neighbor_particles_of_random);
	free(neighbor_particles_at_new);
	
    //if not acceptable then revert the changes
    //use a random_particle acceptance level
    //MONTE CARLO THAT THANG
    if( ((rand())/(RAND_MAX+1.0)) > probability)
    {
       
		//reset the energies in the old cubes
		for(i = 0; i < number_of_old_cubes; i++)
			cubes[old_cube_indices[i]].energy = old_cube_energies[i];	

		//add the particle back to the old location
        addToCube(&cubes[random_particle.myCube], random_particle);

		//remove the particle from the new loaction
        remove_particle(&cubes[temp.myCube], temp);

		//recalculate the energies at the new location
		for(i = 0; i < number_of_adjacents; i++)
            calculate_cube_energy(cubes, adjacents_indices[i]);

		
        //revert
        return 0;
		
    }
    else
	{
		particle_array[index_of_random] = temp;	
        return 1;
	}
}//end perturb

/**
 * Remove a particle from the cube at the given index
 * @param a_cube The cube to be altered
 * @param index The index of the particle in the cube
 * @author Daniel Ward
 */
void remove_particle(cube * a_cube, particle a_particle)
{
	int index = 0;
	char found = 'F';

	//find the index of the the particle to remove
	while((index < a_cube->number_of_particles) && (found != 'T'))
	{

		if((a_cube->particles[index].x == a_particle.x) && 
			(a_cube->particles[index].y == a_particle.y) && 
			(a_cube->particles[index].z == a_particle.z) && 
			(a_cube->particles[index].myCube == a_particle.myCube))
		{
			found = 'T';
		}
		else
		index++;
 
	}

    //swap the last particle for the one to be removed
    a_cube->particles[index] = a_cube->particles[(a_cube->number_of_particles - 1)];

    //expand a temporary array to hold the particles in a_cube minus one
    particle * temp = (particle *)malloc(sizeof(particle) * (a_cube->number_of_particles - 1 ));

    //copy the particles from a_cube to the new array
    int i;
    for(i = 0; i < (a_cube->number_of_particles - 1); i++)
            temp[i] = a_cube->particles[i];

    //free the old array
    free(a_cube->particles);

    //assign the new array to a_cube
    a_cube->particles = temp;

    //reduce the number_of_particles counter
    (*a_cube).number_of_particles--;
}//end remove_particle

/**
 * This will get the particles in the cubes that are in the
 * array of cube indices. Used to get the adjacent particles
 * of a particular cube
 *
 * @param neighbors An array of adjacent cube indices
 * @param num_neighbors The number of adjacent cubes
 * @param cubes An array ofcubes
 * @param total_particles A int to store the number of adjacent particles
 * @return An array of adjacent particles
 * @author Daniel Ward
 */
particle* get_particles_from_cubes(int * neighbors, int num_neighbors, cube * cubes, int * total_particles)
{
    int i, j;//iterators
    int totalParticles = 0;//the total number of particles in the system
    int resultIndex = 0;//the index for the particle array that is returned
    particle * result;//the particle array to be returned

    //go through each neighbor cube to count the particles
    for(i = 0; i < num_neighbors; i++)
    {
        totalParticles += cubes[neighbors[i]].number_of_particles;//increase the total number of particles
    }

    //create a new array for the particles
    result = (particle *)malloc(sizeof(particle)*totalParticles);

    //go through each neighbor cube to get the particles
    for(i = 0; i < num_neighbors; i++)
    {
        //create a temporary reference  to the current cube
        cube temp = cubes[neighbors[i]];

        //add the particles from each cube to the resulting array
        for(j = 0; j < temp.number_of_particles; j++ )
        {
            result[resultIndex] = temp.particles[j];
            resultIndex++;//move the result array index
        }
    }

    //set the total number of particles
    *total_particles = totalParticles;

    //return the array of particles
    return result;
}//end get_particles_from_cubes

/**
 * Check to see if the given cordinates of a cube lies with in the
 * universe
 *
 * @param row The position of the cube on the X axis
 * @param column The position of the cube in th Y axis
 * @param height The position of the cube on the Z axis
 * @return 't' if within the larger cube, else 'f'
 * @author Manuel Zubieta
 */
unsigned char in_bounds(int row, int column, int height){

    //If the cube is in the given domain then return 't'
    if (row>=0 && column>=0 && height>=0 && row<SIZE && column<SIZE && height<SIZE)
            return 't';

    //if not in the domain return 'f'
    return 'f';
}//end in_bounds

/**
 * Finds the cube that a particle belongs to.
 * It uses the coordinates of the particle to
 * find the appropritate cube number
 *
 * @param x The x coordinate of the particle
 * @param y The y coordinate of the particle
 * @param z The z coordinate of the particle
 * @return The cube number that the particle belongs to
 * @author Manuel Zubieta
 */
int belongs_to_cube(double x, double y, double z){

    return ((x) * SIZE) + (y) + ( (z) *(SIZE*SIZE));
}

/**
 * Determine the adjacent cubes of a given cube index.
 *
 * Returns the number of adjacent cubes (including the given cube) and writes the list of adjacents to *adjs
 *
 * @param adjacent_array The array to store the indices of the adjacent cubes
 * @param index the index of the cube that in the base of the lookup
 * @author Manuel Zubieta
*/
int adjacents(int *adjacent_array, int index){
    //adjs[0]=index;          // Whichever cube we're looking for the adjs of, should also be included in the list
    int x = index%(SIZE*SIZE)/SIZE;
    int y = index%SIZE;
    int z = index/(SIZE*SIZE);
    
    int n = 0;
    
    // This code is trivial beyond this point...
    // Check in every possible direction, from the given x,y,z coordinate, if
    // it is 'in the box' and if so, add the corresponding index to the array of adjs
    
    if (in_bounds(x+1,y,z) == 't'){//+__
        adjacent_array[n] = (belongs_to_cube(x+1,y,z));
        n++;
    }
    if (in_bounds(x+1,y+1,z) == 't'){//++_
        adjacent_array[n] = (belongs_to_cube(x+1,y+1,z));
        n++;
    }
    if (in_bounds(x+1,y-1,z) == 't'){//+-_
        adjacent_array[n] = (belongs_to_cube(x+1,y-1,z));
        n++;
    }
    if (in_bounds(x+1,y,z+1) == 't'){//+_+
        adjacent_array[n] = (belongs_to_cube(x+1,y,z+1));
        n++;
    }
    if (in_bounds(x+1,y,z-1) == 't'){//+_-
        adjacent_array[n] = (belongs_to_cube(x+1,y,z-1));
        n++;
    }
    if (in_bounds(x-1,y,z) == 't'){//-__
        adjacent_array[n] = (belongs_to_cube(x-1,y,z));
        n++;
    }
    if (in_bounds(x-1,y+1,z) == 't'){//-+_
        adjacent_array[n] = (belongs_to_cube(x-1,y+1,z));
        n++;
    }
    if (in_bounds(x-1,y-1,z) == 't'){//--_
        adjacent_array[n] = (belongs_to_cube(x-1,y-1,z));
        n++;
    }
    if (in_bounds(x-1,y,z+1) == 't'){//-_+
        adjacent_array[n] = (belongs_to_cube(x-1,y,z+1));
        n++;
    }
    if (in_bounds(x-1,y,z-1) == 't'){//-_-
        adjacent_array[n] = (belongs_to_cube(x-1,y,z-1));
        n++;
    }
    if (in_bounds(x,y+1,z) == 't'){//_+_
        adjacent_array[n] = (belongs_to_cube(x,y+1,z));
        n++;
    }
    if (in_bounds(x,y+1,z+1) == 't'){//_++
        adjacent_array[n] = (belongs_to_cube(x,y+1,z+1));
        n++;
    }
    if (in_bounds(x,y+1,z-1) == 't'){//_+-
        adjacent_array[n] = (belongs_to_cube(x,y+1,z-1));
        n++;
    }
    if (in_bounds(x,y-1,z) == 't'){//_-_
        adjacent_array[n] = (belongs_to_cube(x,y-1,z));
        n++;
    }
    if (in_bounds(x,y-1,z+1) == 't'){//_-+
        adjacent_array[n] = (belongs_to_cube(x,y-1,z+1));
        n++;
    }
    if (in_bounds(x,y-1,z-1) == 't'){//_--
        adjacent_array[n] = (belongs_to_cube(x,y-1,z-1));
        n++;
    }
    if (in_bounds(x,y,z+1) == 't'){//__+
        adjacent_array[n] = (belongs_to_cube(x,y,z+1));
        n++;
    }
    if (in_bounds(x,y,z-1) == 't'){//__-
        adjacent_array[n] = (belongs_to_cube(x,y,z-1));
        n++;
    }
    if (in_bounds(x+1,y+1,z+1) == 't'){//+++
        adjacent_array[n] = (belongs_to_cube(x+1,y+1,z+1));
        n++;
    }
    if (in_bounds(x-1,y+1,z+1) == 't'){//-++
        adjacent_array[n] = (belongs_to_cube(x-1,y+1,z+1));
        n++;
    }
    if (in_bounds(x+1,y-1,z+1) == 't'){//+-+
        adjacent_array[n] = (belongs_to_cube(x+1,y-1,z+1));
        n++;
    }
    if (in_bounds(x+1,y+1,z-1) == 't'){//++-
        adjacent_array[n] = (belongs_to_cube(x+1,y+1,z-1));
        n++;
    }
    if (in_bounds(x-1,y+1,z-1) == 't'){//-+-
        adjacent_array[n] = (belongs_to_cube(x-1,y+1,z-1));
        n++;
    }
    if (in_bounds(x-1,y-1,z+1) == 't'){//--+
        adjacent_array[n] = (belongs_to_cube(x-1,y-1,z+1));
        n++;
    }
    if (in_bounds(x+1,y-1,z-1) == 't'){//+--
        adjacent_array[n] = (belongs_to_cube(x+1,y-1,z-1));
        n++;
    }
    if (in_bounds(x-1,y-1,z-1) == 't'){//---
        adjacent_array[n] = (belongs_to_cube(x-1,y-1,z-1));
        n++;
    }

    return n;
}//end adjacents
