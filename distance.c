#include "pib.h"
#include <math.h>

double distance(particle *a, particle *b){
    return (double)sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2) + pow(a->z - b->z, 2));
}
