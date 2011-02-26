#include "pib.h"

double distance(particle *a, particle *b){

	double deltaX = (*b).x - (*a).x;
	double deltaY = (*b).y - (*a).y;
	double deltaZ = (*b).z - (*a).z;

    return (double)sqrt(deltaX*deltaX + deltaY*deltaY + deltaZ*deltaZ);
}

int positiveVector(particle *a, particle *b){
	//double x = (*b).x - (*a).x ;
	double y = (*b).y - (*a).y ;
	//double z = (*b).z - (*a).z ;
	int result = 0;
	
	
 	if(y > 0)
		result = 1;

	return result;
}
