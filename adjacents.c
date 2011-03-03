#include "pib.h"



int perturb(cube * cubes)
{
	int i; //iterator
	double temp_x, temp_y, temp_z; //used to store the coordinates of the destroyed particles
	int new_cube; //the new cube number for the particle
	int old_cube; //the old cube number for the particle
	

	do{
		old_cube = ((rand())/(RAND_MAX+1.0))*TOTAL_NUMBER_OF_CUBES;
		//printf("cube: %3d num %d\n", old_cube,cubes[old_cube].number_of_particles);
	}while(cubes[old_cube].number_of_particles < 1);

	int index = ((rand())/(RAND_MAX+1.0))*cubes[old_cube].number_of_particles;
	long double old_energy = system_energy(cubes);

 	
	//get the intitial energy at the old cube
	int adjacents_indices[MAX_NUMBER_OF_ADJACENTS];
    int number_of_adjacents = adjacents(adjacents_indices, old_cube);
	long double energy_at_old_initial = caluclate_cube_list_energy(cubes,adjacents_indices, number_of_adjacents);


	//store the initial information of the particle that will be moved
	temp_x = cubes[old_cube].particles[index].x;
	temp_y = cubes[old_cube].particles[index].y;
	temp_z = cubes[old_cube].particles[index].z;

	//remove the particle
	remove_particle(&cubes[old_cube], index);

	//create a new particle to be added to a random cube/////////////
	particle temp;
	
	double x,y,z;
	double random_num = rand();
    x = SIZE*LENGTH_OF_CUBE*((random_num)/(RAND_MAX+1.0));
    temp.x = x;

    random_num = rand();
    y = SIZE*LENGTH_OF_CUBE*((random_num)/(RAND_MAX+1.0));
    temp.y = y;

    random_num = rand();
    z = SIZE*LENGTH_OF_CUBE*((random_num)/(RAND_MAX+1.0));
    temp.z = z;

    temp.myCube = belongs_to_cube((int) x/10,(int) y/10,(int) z/10);

	new_cube = temp.myCube;

    addToCube(&cubes[temp.myCube], temp);
	//////////////////////////////////////////////////////////////////

	
	//recalulate the energy at the old cube location
	for(i = 0; i < number_of_adjacents; i++)
		calculate_cube_energy(cubes, adjacents_indices[i]);

	//get the final energy of the old cube location
	long double energy_at_old_final = caluclate_cube_list_energy(cubes,adjacents_indices, number_of_adjacents);


	//get the information for the new location	
	//adjacents_indices[MAX_NUMBER_OF_ADJACENTS];
    number_of_adjacents = adjacents(adjacents_indices, temp.myCube);

	
	//get the intitial energy at the new cube
	long double energy_at_new_initial = caluclate_cube_list_energy(cubes,adjacents_indices, number_of_adjacents);

	//recalculate the enrgy at the new cube
	for(i = 0; i < number_of_adjacents; i++)
		calculate_cube_energy(cubes, adjacents_indices[i]);

	//get the new energy of the new cube location
	long double energy_at_new_final = caluclate_cube_list_energy(cubes,adjacents_indices, number_of_adjacents);;

	
	//get the new energy of the system
	long double new_energy = old_energy + (energy_at_old_final - energy_at_old_initial) + (energy_at_new_final - energy_at_new_initial);

	//calulate the probability of acceptance
	double probability = compare_energies(old_energy,new_energy,TEMPERATURE);

	//if not acceptable then revert the changes
	if(probability < ACCEPTABLE_PROBABILITY)
	{
		particle temp;
	
		
		temp.x = temp_x;
		temp.y = temp_y;
		temp.z = temp_z;

		temp.myCube = old_cube;

		addToCube(&cubes[old_cube], temp);
		
		remove_particle(&cubes[new_cube],cubes[new_cube].number_of_particles);
		//revert
		return 0;
		
	}
	else
	{
		return 1;
	}
	

}




void remove_particle(cube * a_cube, int index)
{
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

	(*a_cube).number_of_particles--;
}

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

	//go through each neighbor cube
	for(i = 0; i < num_neighbors; i++)
	{
		totalParticles += cubes[neighbors[i]].number_of_particles;//increase the total number of particles
	}

	result = (particle *)malloc(sizeof(particle)*totalParticles);

	for(i = 0; i < num_neighbors; i++)
	{	
		cube temp = cubes[neighbors[i]];
		//add the particles from each cube to the resulting array
		for(j = 0; j < temp.number_of_particles; j++ )
		{
			result[resultIndex] = temp.particles[j];
			resultIndex++;
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
 * @author ????
 */
int belongs_to_cube(double x, double y, double z){

    return ((x) * SIZE) + (y) + ( (z) *(SIZE*SIZE));
	//return (int)( floor(x) / LENGTH_OF_CUBE) + ( floor(y)/LENGTH_OF_CUBE) + ((floor(z)/LENGTH_OF_CUBE) * (SIZE*SIZE));
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
