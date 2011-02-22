#include "pib.h"

#define SIZE_OF(x) (sizeof (x) / sizeof *(x))

void addToCube(cube * aCube, particle aParticle){

	//malloc to increase cube
	if(sizeof(aCube->particles) == 0)
		aCube->particles = (particle *)malloc(sizeof(particle));
	else
		aCube->particles = (particle *)realloc(aCube->particles , SIZE_OF(aCube->particles)+1);

	aCube->particles[SIZE_OF(aCube->particles)-1] = aParticle;
}

int main()
{
	return 0;
}
