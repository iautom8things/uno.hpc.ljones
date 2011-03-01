#include "pib.h"



int perturb(cube * cubes)
{
	int i;
	double temp_x, temp_y, temp_z;
	int new_cube;
	int old_cube;

	do{
		old_cube = rand() % TOTAL_NUMBER_OF_CUBES;
	}while(cubes[old_cube].number_of_particles == 0);

	int index = (int)rand() % cubes[old_cube].number_of_particles;
	long double old_energy = system_energy(cubes);

 	//////////////////////////////////////////////////////////////////
	long double energy_at_old_initial = 0.0;
	int adjacents_indices[MAX_NUMBER_OF_ADJACENTS];
    int number_of_adjacents = adjacents(adjacents_indices, old_cube);

	for(i = 0; i < number_of_adjacents; i++)
		energy_at_old_initial = cubes[adjacents_indices[i]].energy;

	long double energy_at_new_initial = 0.0;

	for(i = 0; i < number_of_adjacents; i++)
		energy_at_new_initial = cubes[adjacents_indices[i]].energy;
	//////////////////////////////////////////////////////////////////

	temp_x = cubes[old_cube].particles[index].x;
	temp_y = cubes[old_cube].particles[index].y;
	temp_z = cubes[old_cube].particles[index].z;

	remove_particle(&cubes[old_cube], index);

	//create a new particle to be added to a random cube;
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

	

	for(i = 0; i < number_of_adjacents; i++)
		calculate_cube_energy(cubes, adjacents_indices[i]);

	/////////////////////////////////////////////////////////////////
	long double energy_at_old_final = 0.0;

	for(i = 0; i < number_of_adjacents; i++)
		energy_at_old_final = cubes[adjacents_indices[i]].energy;
	/////////////////////////////////////////////////////////////////
	adjacents_indices[MAX_NUMBER_OF_ADJACENTS];
    number_of_adjacents = adjacents(adjacents_indices, temp.myCube);

	for(i = 0; i < number_of_adjacents; i++)
		calculate_cube_energy(cubes, adjacents_indices[i]);

	long double energy_at_new_final = 0.0;

	for(i = 0; i < number_of_adjacents; i++)
		energy_at_new_final = cubes[adjacents_indices[i]].energy;


	

	long double new_energy = old_energy + (energy_at_old_final - energy_at_old_initial) + (energy_at_new_final - energy_at_new_initial);
	double probability = compare_energies(old_energy,new_energy,TEMPERATURE);

	if(probability > ACCEPTABLE_PROBABILITY)
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
	int length = a_cube->number_of_particles;

	int i; 
	
	for(i = index; i < length - 1; i++)
	{
		a_cube->particles[i] = a_cube->particles[i+1];
	}

	(*a_cube).number_of_particles--;

	a_cube->particles = (particle *)realloc(a_cube->particles , sizeof(particle)*a_cube->number_of_particles);
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
	result = (particle *)malloc(0);//initialize the array to zero elements

	//go through each neighbor cube
	for(i = 0; i < num_neighbors; i++)
	{
		cube * tempCube = &cubes[neighbors[i]];//get the first cube
		totalParticles += (*tempCube).number_of_particles;//increase the total number of particles
		result = (particle *)realloc(result , sizeof(particle)*totalParticles);//grow the array to hold the new particles

		//add the particles from each cube to the resulting array
		for(j = 0; j < (*tempCube).number_of_particles; j++ )
		{
			result[resultIndex] = tempCube->particles[j];
			resultIndex ++;
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
