#include "pib.h"

#define SIZE_OF(x) (sizeof (x) / sizeof *(x))

void addToCube(cube * aCube, part * aParticle){
    aCube->lastPart+=1;
    
    printf("%d", aCube->lastPart);
	//malloc to increase cube
	if(aCube->lastPart == 0)
		aCube->part = (part *)malloc(sizeof(part));
	else
		aCube->part = (part *)realloc(aCube->part , sizeof(part)*aCube->lastPart + sizeof(part));

	aCube->part[aCube->lastPart] = *aParticle;
    // added by manuel
}
