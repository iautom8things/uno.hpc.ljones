/*
 * @author: Nam Truong (Theo)
 * @email: chocobos8687@gmail.com or nhtruong@uno.edu
 * @Start date: 03/01/2011;
 * @Current version: 1.31 (3/3/11)
 *
 * WARNING! The program uses long double datatype. Some machines in the computer
 * lab can't handle/display this datatype correctly.
 *
 * NOTE_1: The program might appear to run slower over time. However, this is due
 * to the fact that as the system approaches its equilibrium, it becomes harder
 * to perturb. The program will have to try to move a particle more times
 * than at the begining of the simulation. If you set M = 100,000 or more, it
 * might take hours for the program to complete but the result should be
 * a state of energy that is very close to the perfect equilibrium.
 *
 * NOTE_2: The PNR index displayed at the end of each line on the screen is
 * comprised of 3 numbers: accepted positive dE, accepted negative dE and
 * rejected dE of the cycle. You will notice that the number of rejected dE per
 * cycle grows exponentially large over time. This proves that an appropriate dE
 * is harder to find as the system approaches its equilibrium. Each cycle will,
 * therefore, take longer to complete.
 *
 * NOTE_3: For the current setting (Cube Length L = 10), you will notice that
 * the system NEVER accepts any positive dE. Even if you set L = 8000, the number
 * of accepted positive dE(s) only account for less then 1% of the total number
 * of dE(s) accepted.
 *
 * NOTE_4: The initEng() function calculates the total energy by definition (i.e
 * it's no shortcut) and is the only way to calculate the initial energy.
 * It calculates the total energy by adding up the energy of every possible pair
 * in the box. Though it could be used to calculate the new total energy after
 * a particle is moved, there is a shortcut that I can prove to be equally precise
 * but tens of thousands of times more effecient. Since I've already discussed
 * this shortcut with most of you before, and we all can agree that if it actually
 * works, the shortcut will significantly improve the system's perforamce. All
 * we have to do is show that it does work.
 * 
 * To do this, we must show that the difference is negligible:
 * Uncomment the commented line of code inside the for loop of the main function.
 * Now the new total energy will be calculated twice after each cycle: one using
 * the shortcut and one using the definition. You should notice that the
 * differences between the two sets of result is just one over a trillion of a
 * unit of energy (due to rounding). Moveover, each cycle is equal to 100 moves
 * (which is the current setting of the system for steps per cycle). So these
 * tiny tiny differences you see in the result are actually the accumilative
 * differences after 100 steps at a time. That is, the actually margin of error
 * is even much smaller. So, it's "pretty" precise enough, yes?
 *
 *******************************************************************************
 * New in v1.31 (3/3/11):
 * _ Memory leak in perturb() fixed.
 * _ Sigma's and Epilon's values were swapped. Now corrected
 * _ More optimization in perturb() for Density and Probability calculations
 * _ Energy variables are now long double to be able to track very small changes
 *   in total energy.
 * _ Three new variables are introduced. They are not used to debug but rather
 *   to demonstrate why the system slows down over time, and why positive 
 *   changes in total energy are of little help in this simulation.
 *
 *******************************************************************************
 * New in v1.12 (3/2/11):
 * _ relatedCubes() now can handle calls from perturb() also.
 * _ Selection for next particle to perturb has been optimized for low density
 * systems.
 * _ M's and N's datatype has been changed from int to long int
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <process.h>

/*
 * In this program, particles of the same cube are linked together. Each cube
 * only stores the addresses of the first and the last particles. This
 * information is then collected onto an array called box[C][C][C][2] where C is
 * the number of cubes per side (i.e. each element of box[C][C][C] is a cube).
 * Note that box is a 4-dimension array not 3. Like I mentioned above, each cube
 * stores 2 addresses, hence we need a 4th dimension with a size of 2.
 */
typedef struct Particle {
    long double x;
    long double y;
    long double z;
    struct Particle *next;
} Part; // data structure for particles
typedef Part *ptPart; // alias for pointers to Particles

//* Below are constants from the problem description. They should not be changed
const long double K = 1.38; // Boltzmann’s constant
const long double E = 0.2;  // Epsilon
const long double S = 4;    // Sigma

//* Below are constatns that define the system's settings
const long double T = 45000;// Temperature
const long double P = 0.8;  // Acceptance Probability
const long double L = 10;   // Length of a cube in Angstrom
const int         C = 5;    // Cubes per dimention
const long int    M = 5000; // Number of Moves
const long int    N = 1000; // Number of Particles
const int      High = 5;    // Definition of high density
const int     Cycle = 100;  // Number of steps in a cycle (exam main function)

//* MAX and DEN are concrete formulas, NOT a part of the system's setting!!!
#define MAX -log(P) * T * K * pow(10, -23) // Max positive dE accepted
#define DEN N/C/C/C // The system's relative density (particles per cube)

//* Below are variables used for demo/debug purpose. Removing segments of code
//  related to these variables may improve the program's performance though not
//  by much.
int     negaAcpt = 0; // negative dE(s) accepted in the current cycle
int     posiAcpt = 0; // positive dE(s) accepted in the current cycle
int     rejected = 0; // positive dE(s) rejected in the current cycle

/*disCube() is a debug function. It will display every particles in a given cube.
-----------------------------------------------------------------------------*/
void disCube(ptPart cube){
    if(cube == NULL)
        return;
    Part part = *cube;
    int count = 0;
    while(1){
        printf("\t%d: %2.2Lf %2.2Lf %2.2Lf\n", count++, part.x,part.y,part.z);
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
                        printf("\t %2.2Lf %2.2Lf %2.2Lf\n", part.x,part.y,part.z);
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
        part->x = x*L + (L*rand()/(RAND_MAX+1.0));
        part->y = y*L + (L*rand()/(RAND_MAX+1.0));
        part->z = z*L + (L*rand()/(RAND_MAX+1.0));
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
long double part2part(Part partA, Part partB) {
    long double X = partA.x - partB.x;
    long double Y = partA.y - partB.y;
    long double Z = partA.z - partB.z;
    Z = sqrt(X*X+Y*Y+Z*Z);
    Z = S/Z;
    Z = Z*Z*Z;
    Z = Z*Z;
    Z = 4 * E * Z * (Z - 1);
    return Z;
}
/* cube2cube() returns the total energy of between particles of 2 cubes (i.e.
* energy of one particle of the first cube with another particle of the second
* cube).
-----------------------------------------------------------------------------*/
long double cube2cube(ptPart cubeA, ptPart cubeB) {
    if(cubeA == NULL || cubeB == NULL)
        return 0.0;
    long double energy = 0.0;
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
long double inCube(ptPart cube) {
    if(cube == NULL)
        return 0.0;
    if(cube->next == NULL)
        return 0.0;
    long double energy = 0.0;
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
* _These are all cubes surrounding a given cube
* _This mode is used in perturb();
*
* else, the function returns at most 13 cubes
* _ These are the cubes that surrounds a given cube AND haven't been
* paired with this cube yet.
* _ This mode is used in initEng();
*
-----------------------------------------------------------------------------*/
int relatedCubes(ptPart cubes[26], ptPart box[C][C][C][2], int x, int y, int z, int mode) {
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
    if(mode ==0)
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
_ It then calculates the total energy generated by the interaction of the cube
with the related cubes by calling cube2cube() on given pairs of cubes.
-----------------------------------------------------------------------------*/
long double initEng(ptPart box[C][C][C][2]) {
    long double energy = 0.0;
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
long double part2cube(ptPart part, ptPart cube) {
    long double energy = 0;
    if(cube == NULL)
        return 0;
    while(cube != NULL) {
        energy += part2part(*part, *cube);
        cube = cube->next;
    }
    return energy;
}
/*perturb() tries to move a particle until it achieves a desirable change in
total Energy dE. It then returns this dE.Here's what the function does:

1. Select a non-empty cube randomly (x,y,z)
2. Remove the first particle of the selected cube.
3. Calculate the energy contributed by the removed particle
4. Subtract it from dE (energy lost)
5. Select a new cube randomly (X,Y,Z).
6. Generate a particle inside the cube (the particle is yet to be added to the cube)
7. Calculate the energy contributed by the new particle
8. Add it to dE (energy gained)
9. Check for probability based on dE
10. If P is accepted add the new particle to the end of the new cube. Else add
    the old particle to the end of the old cube
    (This ensures that a particle will have less chances to be selected again if
    it has been moved recently. Generally, its chance increases over time).
-----------------------------------------------------------------------------*/
long double perturb(ptPart box[C][C][C][2]){
    ptPart related[26], old, new;
    long double dE;
    int x, y, z, X,Y,Z, size, i;
    while(1){
        dE = 0.0;
        new = (ptPart) malloc(sizeof(Part));

        //* Select a cube using one of the two methods
        if(DEN >= High){ //* high density: full random selection
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
        //* old now points to the first particle of the selected cube
        old = box[x][y][z][0];
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
        new->x = X*L + (L*rand()/(RAND_MAX+1.0));
        new->y = Y*L + (L*rand()/(RAND_MAX+1.0));
        new->z = Z*L + (L*rand()/(RAND_MAX+1.0));
        new->next = NULL;
        //* calculate the total energy contributed by the new particle if added.
        size = relatedCubes(related, box, X,Y,Z, 1);
        dE += part2cube(new, box[X][Y][Z][0]);
        for(i = 0; i < size; i++)
            dE += part2cube(new, related[i]);
        //* Decision: Should the new particle be added?
        if(dE <= MAX) {
            if(dE > 0)
                posiAcpt++;
            else
                negaAcpt++;
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
            rejected++;
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
    return dE;
}
/* And below is the all-glory main function.
 -----------------------------------------------------------------------------*/
int main() {
    ptPart box [C][C][C][2];
    long int i;
    int step = 0;
    long double energy[M+1];
    srand((unsigned) time(NULL));
    clearBox(box);
    initBox(box);
    printf("\n\tA NEW BOX HAS BEEN CREATED AND FILLED WITH %d PARTICLES\n\n", N);
    //printf("\n Total particles: %d", disBox(box)); //enable this line to debug
    energy[0] = initEng(box);
    printf("\n Ini E = %36.20Lf   MAX +dE accepted:  %.3LG\n\n", energy[0], MAX);
    
    for(i = 1; i < M+1; i++) {
        energy[i] = energy[i-1] + perturb(box);
        if(++step == Cycle) {
            printf(" New E = %36.20Lf   i = %4d", energy[i],i);
            printf("   PNR:%2d - %3d - %d\n", posiAcpt, negaAcpt, rejected);

            //printf(" New E*= %36.20Lf\n\n", initEng(box));
            //enable the above line to compare the result of two different methods
            //of calculating the new total energy.

            step = negaAcpt = rejected = posiAcpt = 0;
        }
    }
    printf("\n\n\t\t Tadaaaaaa! :D \n\n");
    return (EXIT_SUCCESS);
}



