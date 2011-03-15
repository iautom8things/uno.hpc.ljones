#include "pib.h"


/**
 * This main is used to test the progress of the solution to the
 * Lennard-Jones Problem.
 *
 * Code is being worked on by Manuel Zubieta, Daniel Ward, and Aaron Maus
 *
 * This program now takes arguments as follows
 * required:
 * -temp THE TEMPERATUR IN KELVIN
 * -trials THE NUMBER OF TRIALS
 * -seed THE SEED FOR THE RANDOM NUMBER GENERATOR
 * optional:
 * -o THE OUTPUT FILE NAME
 */
int main(int argc, char** argv){
    MPI_Status status;
    MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	
    int seed;//seed for the random number generator
    char* output_file;
    if (id==0)
        output_file = "data.csv";//the default output file name


    ///////////   THIS IS FOR YOU DR. SUMMA  //////////////////////
    ///////////         ARGUMENTS            //////////////////////
    
    int i;
    int arguments_found = 0;

    for(i = 0; i < argc; i++){

        if(strcmp(argv[i],"-temp") == 0)
        {
                TEMPERATURE = atoi(argv[i+1]);
                arguments_found++;
        }
        if(strcmp(argv[i],"-trials") == 0)
        {
                NUMBER_OF_TRIALS = atoi(argv[i+1]);
                arguments_found++;
        }
        if(strcmp(argv[i],"-seed") == 0)
        {
                seed = atoi(argv[i+1]); //might want to differentiate the seeds after the initial setup of the box
                arguments_found++;
        }
        if(strcmp(argv[i],"-o") == 0)
        {
                if (id==0)
                    output_file = argv[i+1];
        }
    }

    if(arguments_found != 3)
    {
        printf("Invalid arguments!!!!!\n");
        printf("Please enter the arguments\n");
        printf("-temp TEMP\n");
        printf("-trials NUMBER OF TRIALS\n");
        printf("-seed THE SEED FOR THE RANDOM NUMBER GENERATOR\n");
        exit(0);
    }

	if(NUMBER_OF_TRIALS < 20)
	{
		printf("The number of trials should be >= 20\n");
		exit(0);
	}

    /////////  END ARGUMENTS /////////////////////////////////
		
    srand(seed); // seed the random number generator
    double random_num = rand(); //get a random number


	//double energies[NUMBER_OF_TRIALS];//store accepted energies
    cube cubes[TOTAL_NUMBER_OF_CUBES]; // allocate an array for our cubes

    //initalize the cubes in the array
    for(i=0;i<TOTAL_NUMBER_OF_CUBES;i++){
        cubes[i].number_of_particles = 0;
        cubes[i].energy = 0;
        cubes[i].particles = (particle *)malloc(0);
    }

    particle temp_particle;
    double x,y,z; //our temp variables for the x,y,and z coords

    /*
     * generate the particles and add then to the cube
     * that contains the x,y,z coordinates of the
     * ith particle
     */
    for(i=0;i<NUMBER_OF_PARTICLES;i++){
        random_num = rand();
        x = SIZE*LENGTH_OF_CUBE*((random_num)/(RAND_MAX+1.0));
        temp_particle.x = x;

        random_num = rand();
        y = SIZE*LENGTH_OF_CUBE*((random_num)/(RAND_MAX+1.0));
        temp_particle.y = y;

        random_num = rand();
        z = SIZE*LENGTH_OF_CUBE*((random_num)/(RAND_MAX+1.0));
        temp_particle.z = z;

        temp_particle.myCube = belongs_to_cube((int) x/10,(int) y/10,(int) z/10);

		particle_array[i] = temp_particle;

        addToCube(&cubes[temp_particle.myCube], temp_particle);
    }

    /*
     * calculate the energy of the ith cube and print some information
     * about the cube.
     */
    for(i=0; i< TOTAL_NUMBER_OF_CUBES; i++){

        calculate_cube_energy(cubes, i);
    }
///////////////////////////////////////////////////
    setup_tree();
///////////////////////////////////////////////////
    printf("done %d\n", id);
    //do some clean up
    clean(cubes);
    MPI_Finalize();
}//end main

/**
 * Frees the allocated memory in the given cubes
 *
 * @param cubes The cubes that need to be cleaned
 * @author Daniel Ward
 */
void clean(cube * cubes)
{
    int i;//iterator

    //free the allocated memory for the particles
    for(i = 0; i < TOTAL_NUMBER_OF_CUBES; i++)
            free(cubes[i].particles);
}//end clean

