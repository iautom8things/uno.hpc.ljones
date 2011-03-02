/*
 * @author: Nam Truong (Theo)
 * @email: chocobos8687@gmail.com or nhtruong@uno.edu
 * @versn: 1.1 (3/2/11)
 * @Created on   : 03-01-2011;
 * @Last Modified: 03-02-2011;
 *
 * New in this version:
 * _ relatedCubes() now can handle calls from perturb() correctly
 * _ Selection for next particle to perturb has been optimized for low density
 *   systems.
 *
 * NOTE: The program might appear to run slower over time. However, this is due
 * to the fact that as the system approaches its equilibrium, it becomes harder to
 * perturb. The program will have to try to move a particle more times
 * than at the begining of the simulation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/* In this program, particles of the same cube are linked together. Each cube
 * only stores the addresses of the first and the last particles. This
 * information is then collected onto an array called box[C][C][C][2] where C is
 * the number of cubes per side (i.e. each element of box[C][C][C] is a cube).
 * Note that box is a 4-dimension array not 3. Like I mentioned above, each cube
 * stores 2 addresses, hence we need a 4th dimension with a size of 2.
 */
typedef struct Particle {
    double x;
    double y;
    double z;
    struct Particle *next;
} Part;
typedef Part *ptPart;
/*  e = Math constant (Euler number)
    K = constant given in the problem description
    T = Temperature (I chose 4500K)
    E = Epsilon
    U = Sigma (I know! It makes more sense to call it S but I already used S for something else.)
    S = size of the cube
    C = number of cubes per dimension of the box
    M = Number of moves
    N = Number of particles
    den = the density (particles per cube) at which full random selection is use.
    Debug = Used for debugging. You will understand when you see it in the code.
*/
const double e = 2.718281828459045;
const double K = 0.0000000000000000000000138;
const double T = 4500;
const double E = 4;
const double U = 0.2;
const double S = 1;   
const int C = 50;
const long int M = 15000;
const int N = 1000;
const int den = 1;
const int Debug = 100;
/*disCube() is a debug function. It will display every particles in a given cube.
 -----------------------------------------------------------------------------*/
void disCube(ptPart cube){
    if(cube == NULL)
        return;
    Part part = *cube;
    int count = 0;
    while(1){
        printf("\t%d: %2.2f  %2.2f  %2.2f\n", count++, part.x,part.y,part.z);
        if(part.next == NULL)
            break;
            part = *(part.next);
    }
}
/*disBox() is also a debug function like disCube(). It displays all particles of
 * the box.
 -----------------------------------------------------------------------------*/
int disBox(ptPart box[C][C][C][2]) {
    int i, j, k;
    int count = 0;
    Part part;
    for(i = 0; i < C; i++)
        for(j = 0; j < C; j++)
            for(k = 0; k < C; k++) {
                if(box[i][j][k][0] != NULL) {
                    part = *(box[i][j][k][0]);
                    printf("\n\n Particle(s) of Cube %d-%d-%d: \n\n", i, j, k);
                    while(1){
                        printf("\t %2.2f  %2.2f  %2.2f\n", part.x,part.y,part.z);
                        count++;
                        if(part.next == NULL)
                            break;
                        part = *(part.next);
                    }
                }
            }
    return count;
}
/*clearBox() clears all pointers stored in the box array (i.e. set them to NULL)
 -----------------------------------------------------------------------------*/
void clearBox(ptPart box[C][C][C][2]) {
    int i, j, k;
    for(i = 0; i < C; i++)
        for(j = 0; j < C; j++)
            for(k = 0; k < C; k++) {
                box[i][j][k][0]= NULL;
                box[i][j][k][1]= NULL;
            }
}
/*initBox() places N particles inside the box. It first randomly pick a cube
 * through the cube's 3 indices on box[][][]. Once a cube is selected, a
 * particle with an appropriate position is placed into the cube
 -----------------------------------------------------------------------------*/
void initBox(ptPart box[C][C][C][2]) {
    int x, y, z, i;
    ptPart part;
    for(i = 0; i < N; i++) {
        part = (ptPart) malloc(sizeof(Part));
        //* Select a cube:
        x = rand() % C;
        y = rand() % C;
        z = rand() % C;

        //* Generate a particle inside the cube:
        part->x = x*S + (S*rand()/(RAND_MAX+1.0));
        part->y = y*S + (S*rand()/(RAND_MAX+1.0));
        part->z = z*S + (S*rand()/(RAND_MAX+1.0));
        part->next = NULL;

        //* Add the particle to the cube:
        if(box[x][y][z][0] == NULL){
            box[x][y][z][1] = part;
            box[x][y][z][0] = part;
        }
        else {
            box[x][y][z][1]->next = part;
            box[x][y][z][1] = part;
        }
    }
}
/* part2part() returns the energy between 2 particles.
 * I avoided using pow() for optimization purpose.
 -----------------------------------------------------------------------------*/
double part2part(Part partA, Part partB) {
    double X = partA.x - partB.x;
    double Y = partA.y - partB.y;
    double Z = partA.z - partB.z;
    Z = sqrt(X*X+Y*Y+Z*Z);
    Z = U/Z;
    Z = Z*Z*Z;
    Z = Z*Z;
    Z = 4 * E * Z * (Z - 1);
    return Z;
}
/* cube2cube() returns the total energy of between particles of 2 cubes (i.e.
 * energy of one particle of the first cube with another particle of the second
 * cube).
 -----------------------------------------------------------------------------*/
double cube2cube(ptPart cubeA, ptPart cubeB) {
    if(cubeA == NULL || cubeB == NULL)
        return 0.0;
    double energy = 0.0;
    Part partA = *cubeA;
    Part partB = *cubeB;
    while(1) {
        energy += part2part(partA, partB);
        if(partA.next == NULL) {
            if(partB.next == NULL)
                break;
            partA = *cubeA;
            partB = *partB.next;
        } else
            partA = *partA.next;
    }
    return energy;
}
/* inCube() returns the total energy generated by pairs of particles within a
 * given cube.
 -----------------------------------------------------------------------------*/
double inCube(ptPart cube) {
    if(cube == NULL)
        return 0.0;
    if(cube->next == NULL)
        return 0.0;
    double energy = 0.0;
    Part curr = *cube;
    Part next = *curr.next;
    while(1) {
        energy += part2part(curr, next);
        if(next.next == NULL) {
            curr = *curr.next;
            if(curr.next == NULL)
                break;
            next = *curr.next;
        } else
            next = *next.next;
    }
    return energy;
}
/* relatedCubes() returns the number of cubes related a given cube. The
 * function call must also provides an array of type ptPart (pointer to particle)
 * called cubes in order to store and retrieve the addresses of the related
 * cubes. The algorithm for this function is rather complicated, but in a nutshell:
 *
 * if all == 1 (True), the function returns at most 26 cubes
 *  _These are all cubes surrounding a given cube
 *  _This mode is used in perturb();
 *
 * else, the function  returns at most 13 cubes
 * _ These are the cubes that surrounds a given cube AND haven't been
 *   paired with this cube yet.
 * _ This mode is used in initEng();
 *
 -----------------------------------------------------------------------------*/
int relatedCubes(ptPart cubes[26], ptPart box[C][C][C][2], int x, int y, int z, int all) {
    int count = 0;
    if(z+1 < C) {
        cubes[count++] = box[x][y][z+1][0];
        if(y+1 < C) {
            cubes[count++] = box[x][y+1][z+1][0];
            if(x+1 < C)
                cubes[count++] = box[x+1][y+1][z+1][0];
            if(x-1 >= 0)
                cubes[count++] = box[x-1][y+1][z+1][0];
        }
        if(y-1 >= 0) {
            cubes[count++] = box[x][y-1][z+1][0];
            if(x+1 < C)
            cubes[count++] = box[x+1][y-1][z+1][0];
            if(x-1 >= 0)
            cubes[count++] = box[x-1][y-1][z+1][0];
        }
        if(x+1 < C)
            cubes[count++] = box[x+1][y][z+1][0];
        if(x-1 >= 0)
            cubes[count++] = box[x-1][y][z+1][0];
    }
    //------------------------------------------------
    if(y+1 < C) {
        cubes[count++] = box[x][y+1][z][0];
        if(x+1 < C)
            cubes[count++] = box[x+1][y+1][z][0];
        if(x-1 >= 0)
            cubes[count++] = box[x-1][y+1][z][0];
    }
    //------------------------------------------------
    if(x+1 < C)
        cubes[count++] = box[x+1][y][z][0];
    //------------------------------------------------
    if(all<=0)
        return count;
    //************************************************
    //************************************************
    if(z-1 >= 0) {
        cubes[count++] = box[x][y][z-1][0];
        if(y+1 < C) {
            cubes[count++] = box[x][y+1][z-1][0];
            if(x+1 < C)
                cubes[count++] = box[x+1][y+1][z-1][0];
            if(x-1 >= 0)
                cubes[count++] = box[x-1][y+1][z-1][0];
        }
        if(y-1 >= 0) {
            cubes[count++] = box[x][y-1][z-1][0];
            if(x+1 < C)
            cubes[count++] = box[x+1][y-1][z-1][0];
            if(x-1 >= 0)
            cubes[count++] = box[x-1][y-1][z-1][0];
        }
        if(x+1 < C)
            cubes[count++] = box[x+1][y][z-1][0];
        if(x-1 >= 0)
            cubes[count++] = box[x-1][y][z-1][0];
    }
    //------------------------------------------------
    if(y-1 >= 0) {
        cubes[count++] = box[x][y-1][z][0];
        if(x+1 < C)
            cubes[count++] = box[x+1][y-1][z][0];
        if(x-1 >= 0)
            cubes[count++] = box[x-1][y-1][z][0];
    }
    //------------------------------------------------
    if(x-1 >= 0)
        cubes[count++] = box[x-1][y][z][0];
    //------------------------------------------------
    return count;
}
/*nonemptyCubes() return the size of an array of indices of cubes with particles
 -----------------------------------------------------------------------------*/
int nonemptyCubes(int cubes[C*C][2], ptPart box[C][C][C][2], int *slice) {
    int i,j,k;
    int count = 0;
    do{
        i = rand() % C;
        for(j = 0; j < C; j++)
            for(k = 0; k < C; k++) {
                if(box[i][j][k][0] != NULL){
                    cubes[count][0] = j;
                    cubes[count][1] = k;
                    count++;
                }
            }
    }while(count == 0);
    (*slice) = i;
    return count;
}
/*initEng() calculates and returns the total energy of the whole system.
_ It traverse the box array to access all cubes.
_ At each cube, it calculates the energy generated within the cube by calling
  inCube().
_ It then calculate the total energy generated by the interaction of the cube
  with the related cubes by calling cube2cube() on given pairs of cubes.
 -----------------------------------------------------------------------------*/
double initEng(ptPart box[C][C][C][2]) {
    double energy = 0.0;
    ptPart cube;
    ptPart related[13];
    int size;
    int x, y, z, i;
    for(z = 0; z < C; z++)
        for(y = 0; y < C; y++)
            for(x = 0; x < C; x++) {
                cube = box[x][y][z][0];
                if(cube != NULL){
                    energy += inCube(cube);
                    size = relatedCubes(related, box, x, y, z, 0);
                    for(i = 0; i < size; i++) {
                        energy += cube2cube(cube, related[i]);
                    }
                }
            }


    return energy;
}
/*part2cube() returns the energy contributed by a given particle when
 * interacting with other particles of a cube.
 -----------------------------------------------------------------------------*/
double part2cube(ptPart part, ptPart cube) {
    double energy = 0;
    if(cube == NULL)
        return 0;
    while(cube != NULL) {
        energy += part2part(*part, *cube);
        cube = cube->next;
    }
    return energy;
}
/*perturb() tries to move a particle until it achieves a desirable change in
  total Energy dE. It then returns this dE. This function may appear long but
  almost half of it is grey-out debug code. Here's what the function does:

1. Select a non-empty cube randomly (x,y,z)
2. Remove the first particle of the selected cube.
3. Calculate the energy contributed by the removed particle
4. Subtract it from dE (energy lost)
5. Select a new cube randomly.
6. Generate a particle inside the cube (the particle is yet to be added to the cube)
7. Calculate the energy contributed by the new particle
8. Add it to dE (energy gained)
9. Check for probability based on dE
10. If P >= 1, add the new particle to the end of the new cube. Else add the old
    particle to the end of the old cube
    (This ensures that a particle will have less chance to be selected again if
    it has been moved recently. Its chance increases as time goes by)
 -----------------------------------------------------------------------------*/
double perturb(ptPart box[C][C][C][2]){
    ptPart related[26], old, new;
    double dE;
    int x, y, z, X,Y,Z, size, i;
    while(1){
        dE = 0.0;
        old = (ptPart) malloc(sizeof(Part));
        new = (ptPart) malloc(sizeof(Part));

        //* Select a cube using one of the two methods
        if(N/C/C/C >= den){ //* high density: full random selection
            do{
                x = rand() % C;
                y = rand() % C;
                z = rand() % C;
            }while(box[x][y][z][0] == NULL);
        } else { //* low density: semi random selection
            int nonempty[C*C][2];
            size = nonemptyCubes(nonempty, box, &x);
            i = rand() % size;
            y = nonempty[i][0];
            z = nonempty[i][1];
        }
        //* copy the value of the first particle to old's address.
        *old = *box[x][y][z][0];
        //* remove the first particle from the cube.
        if(old->next == NULL) {
                box[x][y][z][0] = NULL;
                box[x][y][z][1] = NULL;
        } else
            box[x][y][z][0] = old->next;
        old->next = NULL;
        //* calculate total energy contributed by the old particle
        size = relatedCubes(related, box, x,y,z, 1);
        dE -= part2cube(old, box[x][y][z][0]);
        for(i = 0; i < size; i++)
            dE -= part2cube(old, related[i]);
        //* randomly select a cube for the new particle
        X = rand() % C;
        Y = rand() % C;
        Z = rand() % C;
        //* assign a random position inside the cube for the new particle
        new->x = X*S + (S*rand()/(RAND_MAX+1.0));
        new->y = Y*S + (S*rand()/(RAND_MAX+1.0));
        new->z = Z*S + (S*rand()/(RAND_MAX+1.0));
        new->next = NULL;
        //* calculate the total energy contributed by the new particle if added.
        size = relatedCubes(related, box, X,Y,Z, 1);
        dE += part2cube(new, box[X][Y][Z][0]);
        for(i = 0; i < size; i++)
            dE += part2cube(new, related[i]);
        //* Decision: Should the new particle be added?
        if(dE <= 0 || pow(e, -dE/K/T) >= 1) {
            free(old);
            if(box[X][Y][Z][0] == NULL) {
                box[X][Y][Z][0] = new;
                box[X][Y][Z][1] = new;
            } else {
                box[X][Y][Z][1]->next = new;
                box[X][Y][Z][1] = new;
            }
            break;
        } else {
            free(new);
            if(box[x][y][z][0] == NULL) {
                box[x][y][z][0] = old;
                box[x][y][z][1] = old;
            } else {
                box[x][y][z][1]->next = old;
                box[x][y][z][1] = old;
            }
        }
    }
    //printf("Energy changed: %f", dE);
    return dE;
}
/* And below is the all-glory main function.
 -----------------------------------------------------------------------------*/
int main() {
    ptPart box [C][C][C][2];
    long int i;
    int skip = 0;
    double energy[M+1];
    srand((unsigned) time(NULL));
    clearBox(box);
    initBox(box);
    //printf("\n Total particles: %d", disBox(box)); //enable this line to debug
    energy[0] = initEng(box);
    printf("\n\n\n Initial Energy = %f\n\n", energy[0]);
    
    for(i = 1; i < M+1; i++) {
        energy[i] = energy[i-1] + perturb(box);
        if(++skip == Debug) {
            printf("\t New E  = %f \ti = %d\n", energy[i],i);
            skip = 0;
        }
    }
    //printf("\n Total particles: %d", disBox(box)); //enable this line to debug
    printf("\n\t\t Tadaaaaaa! :D \n\n");
    return (EXIT_SUCCESS);
}



