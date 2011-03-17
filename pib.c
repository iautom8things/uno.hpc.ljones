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
                seed = atoi(argv[i+1])+id; //might want to differentiate the seeds after the initial setup of the box
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
    //cube cubes[TOTAL_NUMBER_OF_CUBES]; // allocate an array for our cubes

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
    int max_buff_size = 4;
    int childrens_max_buff_size = 4;
    if (id != 1 && id != 0){
        max_buff_size = floor(log2(id))*4; // There's 4 double elements 
        childrens_max_buff_size = max_buff_size+4;
    }
    double previous_state[max_buff_size];
    double accepted_state[childrens_max_buff_size]; 
    double rejected_state[childrens_max_buff_size];
    
    setup_tree(max_buff_size, childrens_max_buff_size, previous_state, accepted_state, rejected_state);
    
    /*if (id==0){
        //start the simualtion
        printf("Starting the simulation with %d trials\n", NUMBER_OF_TRIALS);

        i = 0;//reset the iterator to use the while loop
        int unaccepted = 0;
        int accepted = 0;


        //open a file to save the energies, this makes it easier to graph
        FILE * file = fopen(output_file,"w");

        //do until we have the number of successes we are looking for
        while(i < NUMBER_OF_TRIALS)
        {
            //the energy of the current state of the cube
            long double old_energy = system_energy(cubes);

            //if the perturb creates an acceptable state then save the energy
            if(perturb(cubes) == 1)
            {
                //write the energy to file
                fprintf(file,"Energy\t%4d\t%Lf\n",i,old_energy);

                accepted++;

            }//end if

            else
                unaccepted++;//if not an acceptable state then increment the counter

            i++;//move the iterator

            /////////// Fancy-schmancy progress bar :)
            int twentieth = NUMBER_OF_TRIALS/20;
            printf("\r%7d | %3d%% [",i, (i/twentieth)*5);
            int j;
            for (j=0;j<i/twentieth;j++){
                printf("--");
            }

            for (j=0;j<(20-i/twentieth);j++){
                printf("  ");
            }
            printf("] 100%%");
            fflush(0);
            /////////// End fancy-schmancy progress bar
        }//end while

        //close the file
        fclose(file);

        //print the successes and the failures
        printf("\n");
        printf("Accepted States: %d\n", accepted);
        printf("Unaccepted States: %d\n" , unaccepted);
    }//end if main process  */
///////////////////////////////////////////////////
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

