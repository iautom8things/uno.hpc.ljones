#include "pib.h"

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
int adjacents(int *adjs){ // Determine the adjacent squares
    //int adjs[27];  // This array should exist outside of this method, with a pointer to
                        // this array should be a parameter. n (number of adjs) would be returned.
    int index = 62;      // This should actually be a parameter
    adjs[0]=index;  // Whichever cube we're looking for the adjs, will always be in list
    int x = index%(SIZE*SIZE)/SIZE;
    int y = index%SIZE;
    int z = index/(SIZE*SIZE);
    
    int n = 1;
    
    // This code is trivial beyond this point...Actually all of it is trivial
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
