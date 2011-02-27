#include "pib.h"

/**
 * Calulate the distance between two particles
 *
 * @param a The first particle
 * @param b The Second particle
 * @return The distance between a and b
 * @author Manuel Zubieta
 */
double distance(particle a, particle b){

	double deltaX = b.x - a.x;
	double deltaY = b.y - a.y;
	double deltaZ = b.z - a.z;

    return (double)sqrt(deltaX*deltaX + deltaY*deltaY + deltaZ*deltaZ);
}
