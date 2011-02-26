#include "pib.h"



particle* getParticles(int * neighbors, int num_neighbors, cube * cubes, int * total_particles)
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

	return result;
}

// Written by Manuel Zubieta
unsigned char inBounds(int row, int column, int height){ 
    // is the specefied cube (row, columne, height)  on the grid?
	if (row>=0 && column>=0 && height>=0 && row<SIZE && column<SIZE && height<SIZE)
		return 't';
	return 'f';
}
int belongs_to_cube(int x, int y, int z){
    return ( (x) * SIZE) + (y) + ( (z) *(SIZE*SIZE));
}

/**
 * Determine the adjacent cubes of a given cube index.
 *
 * Returns the number of adjacent cubes (including the given cube) and writes the list of adjacents to *adjs
 *
 *   @param adjs
 *   @param index
*/

int adjacents(int *adjs, int index){ 
    adjs[0]=index;          // Whichever cube we're looking for the adjs of, should also be included in the list
    int x = index%(SIZE*SIZE)/SIZE;
    int y = index%SIZE;
    int z = index/(SIZE*SIZE);
    
    int n = 1;
    
    // This code is trivial beyond this point...
    // Check in every possible direction, from the given x,y,z coordinate, if
    // it is 'in the box' and if so, add the corresponding index to the array of adjs
    
    if (inBounds(x+1,y,z) == 't'){//+__
        adjs[n] = (belongs_to_cube(x+1,y,z));
        n++;
    }
    if (inBounds(x+1,y+1,z) == 't'){//++_
        adjs[n] = (belongs_to_cube(x+1,y+1,z));
        n++;
    }
    if (inBounds(x+1,y-1,z) == 't'){//+-_
        adjs[n] = (belongs_to_cube(x+1,y-1,z));
        n++;
    }
    if (inBounds(x+1,y,z+1) == 't'){//+_+
        adjs[n] = (belongs_to_cube(x+1,y,z+1));
        n++;
    }
    if (inBounds(x+1,y,z-1) == 't'){//+_-
        adjs[n] = (belongs_to_cube(x+1,y,z-1));
        n++;
    }
    if (inBounds(x-1,y,z) == 't'){//-__
        adjs[n] = (belongs_to_cube(x-1,y,z));
        n++;
    }
    if (inBounds(x-1,y+1,z) == 't'){//-+_
        adjs[n] = (belongs_to_cube(x-1,y+1,z));
        n++;
    }
    if (inBounds(x-1,y-1,z) == 't'){//--_
        adjs[n] = (belongs_to_cube(x-1,y-1,z));
        n++;
    }
    if (inBounds(x-1,y,z+1) == 't'){//-_+
        adjs[n] = (belongs_to_cube(x-1,y,z+1));
        n++;
    }
    if (inBounds(x-1,y,z-1) == 't'){//-_-
        adjs[n] = (belongs_to_cube(x-1,y,z-1));
        n++;
    }
    if (inBounds(x,y+1,z) == 't'){//_+_
        adjs[n] = (belongs_to_cube(x,y+1,z));
        n++;
    }
    if (inBounds(x,y+1,z+1) == 't'){//_++
        adjs[n] = (belongs_to_cube(x,y+1,z+1));
        n++;
    }
    if (inBounds(x,y+1,z-1) == 't'){//_+-
        adjs[n] = (belongs_to_cube(x,y+1,z-1));
        n++;
    }
    if (inBounds(x,y-1,z) == 't'){//_-_
        adjs[n] = (belongs_to_cube(x,y-1,z));
        n++;
    }
    if (inBounds(x,y-1,z+1) == 't'){//_-+
        adjs[n] = (belongs_to_cube(x,y-1,z+1));
        n++;
    }
    if (inBounds(x,y-1,z-1) == 't'){//_--
        adjs[n] = (belongs_to_cube(x,y-1,z-1));
        n++;
    }
    if (inBounds(x,y,z+1) == 't'){//__+
        adjs[n] = (belongs_to_cube(x,y,z+1));
        n++;
    }
    if (inBounds(x,y,z-1) == 't'){//__-
        adjs[n] = (belongs_to_cube(x,y,z-1));
        n++;
    }
    if (inBounds(x+1,y+1,z+1) == 't'){//+++
        adjs[n] = (belongs_to_cube(x+1,y+1,z+1));
        n++;
    }
    if (inBounds(x-1,y+1,z+1) == 't'){//-++
        adjs[n] = (belongs_to_cube(x-1,y+1,z+1));
        n++;
    }
    if (inBounds(x+1,y-1,z+1) == 't'){//+-+
        adjs[n] = (belongs_to_cube(x+1,y-1,z+1));
        n++;
    }
    if (inBounds(x+1,y+1,z-1) == 't'){//++-
        adjs[n] = (belongs_to_cube(x+1,y+1,z-1));
        n++;
    }
    if (inBounds(x-1,y+1,z-1) == 't'){//-+-
        adjs[n] = (belongs_to_cube(x-1,y+1,z-1));
        n++;
    }
    if (inBounds(x-1,y-1,z+1) == 't'){//--+
        adjs[n] = (belongs_to_cube(x-1,y-1,z+1));
        n++;
    }
    if (inBounds(x+1,y-1,z-1) == 't'){//+--
        adjs[n] = (belongs_to_cube(x+1,y-1,z-1));
        n++;
    }
    if (inBounds(x-1,y-1,z-1) == 't'){//---
        adjs[n] = (belongs_to_cube(x-1,y-1,z-1));
        n++;
    }

    return n;
}
