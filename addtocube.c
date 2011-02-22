#include "pib.h"

#define SIZE_OF(x) (sizeof (x) / sizeof *(x))

void addToCube(cube * aCube, part aParticle){
    
    //printf("%d", aCube->number_of_particles);
	//malloc to increase cube
	if(aCube->number_of_particles ==0)
		aCube->part = (part *)malloc(sizeof(part));
	else		      
		aCube->part = (part *)realloc(aCube->part , sizeof(part)*aCube->number_of_particles + sizeof(part));

	aCube->part[aCube->number_of_particles] = aParticle;
	aCube->number_of_particles++;
}
