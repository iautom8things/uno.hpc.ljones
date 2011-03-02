/*
 * @author: Nam Truong (Theo)
 * @email: chocobos8687@gmail.com or nhtruong@uno.edu
 * @versn: 0.9 (Need more finetuning)(3/2/11)
 * @Created on   : 03-01-2011;
 * @Last Modified: 03-02-2011;
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
typedef struct Particle {
    double x;
    double y;
    double z;
    struct Particle *next;
} Part;
typedef Part *ptPart;
const double e = 2.718281828459045;
const double K = 0.0000000000000000000000138;
const double T = 4500;
const double E = 4;
const double U = 0.2;
const double L = 5;  // length of each side of the box
const double S = 1;   // size of each cube
const int C = 5;        // cubes per side (i.e. L/S)
const int M = 500;    // number of moves
const int N = 100;     //number of particles in the box
const int Debug = 10;     // Debug number

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
void disBox(ptPart box[C][C][C][2]) {
    int i, j, k;
    int invl = 0;
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
                        if(++invl == Debug){
                           invl = 0;
                           getchar();
                        }
                        if(part.next == NULL)
                            break;
                        part = *(part.next);
                    }
                }
            }
     printf("\n\nTotal: %d Particle(s)",count);
}
void clearBox(ptPart box[C][C][C][2]) {
    int i, j, k;
    for(i = 0; i < C; i++)
        for(j = 0; j < C; j++)
            for(k = 0; k < C; k++) {
                box[i][j][k][0]= NULL;
                box[i][j][k][1]= NULL;
            }
}
void initBox(ptPart box[C][C][C][2]) {
    int x, y, z, i;
    ptPart part;
    for(i = 0; i < N; i++) {
        part = (ptPart) malloc(sizeof(Part));
        x = rand() % C;
        y = rand() % C;
        z = rand() % C;
        part->x = x*S + (S*rand()/(RAND_MAX+1.0));
        part->y = y*S + (S*rand()/(RAND_MAX+1.0));
        part->z = z*S + (S*rand()/(RAND_MAX+1.0));
        part->next = NULL;

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
int relatedCubes(ptPart cubes[13], ptPart box[C][C][C][2], int x, int y, int z) {
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
    if(y+1 < C) {
        cubes[count++] = box[x][y+1][z][0];
        if(x+1 < C)
            cubes[count++] = box[x+1][y+1][z][0];
        if(x-1 >= 0)
            cubes[count++] = box[x-1][y+1][z][0];
    }
    if(x+1 < C)
        cubes[count++] = box[x+1][y][z][0];
    return count;
}
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
                    size = relatedCubes(related, box, x, y, z);
                    for(i = 0; i < size; i++) {
                        energy += cube2cube(cube, related[i]);
                    }
                }
            }


    return energy;
}
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
double perturb(ptPart box[C][C][C][2]){
    ptPart related[13], old, new;
    double dE;
    int x, y, z, X,Y,Z, size, i;
    while(1){
        dE = 0.0;
        old = (ptPart) malloc(sizeof(Part));
        new = (ptPart) malloc(sizeof(Part));
        do{
            x = rand() % C;
            y = rand() % C;
            z = rand() % C;
        }while(box[x][y][z][0] == NULL);
/*
        printf("\n\n Particle(s) of Cube %d-%d-%d: (before remove)\n\n", x, y, z);
        disCube(box[x][y][z][0]);
        *old = *box[x][y][z][0];
        printf("\nOLD: %2.2f  %2.2f  %2.2f\n", old->x,old->y,old->z);
*/
        if(old->next == NULL) {
            box[x][y][z][0] = NULL;
            box[x][y][z][1] = NULL;
        } else
            box[x][y][z][0] = old->next;
        old->next = NULL;

/*
        printf("\n\n Particle(s) of Cube %d-%d-%d: (after remove)\n\n", x, y, z);
        disCube(box[x][y][z][0]);
        printf("\nOLD: %2.2f  %2.2f  %2.2f\n", old->x,old->y,old->z);
*/

        size = relatedCubes(related, box, x,y,z);
        dE -= part2cube(old, box[x][y][z][0]);
        for(i = 0; i < size; i++)
            dE -= part2cube(old, related[i]);

        X = rand() % C;
        Y = rand() % C;
        Z = rand() % C;
        new->x = X*S + (S*rand()/(RAND_MAX+1.0));
        new->y = Y*S + (S*rand()/(RAND_MAX+1.0));
        new->z = Z*S + (S*rand()/(RAND_MAX+1.0));
        new->next = NULL;
        size = relatedCubes(related, box, X,Y,Z);
        dE += part2cube(new, box[X][Y][Z][0]);
        for(i = 0; i < size; i++)
            dE += part2cube(new, related[i]);
/*
        printf("\nENERGY CHANGED: %f\n", dE);
*/
        if(dE <= 0 || pow(e, -dE/K/T) >= 1) {
/*
            printf("\n\n Particle(s) of Cube %d-%d-%d: (before add new)\n\n", X, Y, Z);
            disCube(box[X][Y][Z][0]);
            printf("\nNEW: %2.2f  %2.2f  %2.2f\n", new->x,new->y,new->z);
*/
            free(old);
            if(box[X][Y][Z][0] == NULL) {
                box[X][Y][Z][0] = new;
                box[X][Y][Z][1] = new;
            } else {
                box[X][Y][Z][1]->next = new;
                box[X][Y][Z][1] = new;
            }
/*
            printf("\n\n Particle(s) of Cube %d-%d-%d: (after add new)\n\n", X, Y, Z);
            disCube(box[X][Y][Z][0]);
            printf("\nNEW: %2.2f  %2.2f  %2.2f\n", new->x,new->y,new->z);
*/
            break;
        } else {
/*
            printf("\n\n Particle(s) of Cube %d-%d-%d: (before add old)\n\n", x, y, z);
            disCube(box[x][y][z][0]);
            printf("\nOLD: %2.2f  %2.2f  %2.2f\n", old->x,old->y,old->z);
*/
            free(new);
            if(box[x][y][z][0] == NULL) {
                box[x][y][z][0] = old;
                box[x][y][z][1] = old;
            } else {
                box[x][y][z][1]->next = old;
                box[x][y][z][1] = old;
            }
/*
            printf("\n\n Particle(s) of Cube %d-%d-%d: (after add old)\n\n", x, y, z);
            disCube(box[x][y][z][0]);
            printf("\nOLD: %2.2f  %2.2f  %2.2f\n", old->x,old->y,old->z);
*/
        }
    }
    //printf("Energy changed: %f", dE);
    return dE;
}
int main() {
    ptPart box [C][C][C][2];
    int i, skip = 0;
    double energy[M+1];
    srand((unsigned) time(NULL));
    clearBox(box);
    initBox(box);
    //disBox(box); //enable this line to debug
    energy[0] = initEng(box);
    printf("\n\n\n Initial Energy = %f\n\n", energy[0]);
    
    for(i = 1; i < M+1; i++) {
        energy[i] = energy[i-1] + perturb(box);
        if(++skip == Debug) {
            printf("\t New E  = %f\n", energy[i]);
            skip = 0;
        }
    }

    printf("\n\t Tadaaaaaa! :D \n\n");
    return (EXIT_SUCCESS);
}



