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

//	do{

	int max_buff_size = 4;
	int childrens_max_buff_size = 4;

	if (id != 0){
	    max_buff_size = floor(log2(id))*4; // There's 4 double elements 
	    childrens_max_buff_size = max_buff_size+4;
	}

	double current_peturbing[4];
	double previous_state[max_buff_size];
	double accepted_state[childrens_max_buff_size]; 
	double rejected_state[childrens_max_buff_size];
	
	setup_tree(max_buff_size, childrens_max_buff_size, previous_state, current_peturbing, accepted_state, rejected_state);

	
		printf("Updateing state in process: %d\n",id);

		if(id != 0)
	    	update_state(cubes, particle_array, previous_state, current_peturbing, max_buff_size);

		int children_result_length;

		if(id == 0)
		 	children_result_length = 6 * floor(log2(nprocs));
		else
			children_result_length = 6 * ( floor(log2(nprocs)) - floor(log2(id+1)));

		printf("length of process: %d children data: %d\n",id,children_result_length);

		int result_length = children_result_length + 6;

		long double result[result_length];
		long double finished_left[children_result_length];
		long double finished_right[children_result_length];

		for(i = 0; i < result_length; i++)
			result[i] = -1;
		
		printf("Perturbing in process: %d\n",id);

		perturb(current_peturbing, result);	

		//printf("length of data: %d from children ID: %d\n",children_result_length,id);

		for(i = 2; i < 6; i++)
			result[i] = current_peturbing[i-2];

		printf("Perturbing complete in process: %d\n",id);

		int left_child = (2*id)+1;
	    int right_child = (2*id)+2;


		int parent = (id-1)/2;

	
	    // busy wait for childreto finish
	    if (left_child < (nprocs)){

			printf("Wating for process %d left child: %d\n",id,left_child);

	        MPI_Recv(&finished_left, children_result_length, MPI_LONG_DOUBLE, left_child, left_child, MPI_COMM_WORLD, &status);
		
		}
	    if (right_child < (nprocs)){

			printf("Waiting for process %d right child: %d\n",id,right_child);

	        MPI_Recv(&finished_right, children_result_length, MPI_LONG_DOUBLE, right_child, right_child, MPI_COMM_WORLD, &status);
		}
	
		//if i succeed pick the left childs data
		if(result[0] == 1 && (left_child < nprocs))
		{
			for(i = 0; i < children_result_length; i++)
				result[i+6] = finished_left[i];
				
		}

		//if i fail then pick the right child data
		if(result[0] == 0 && (right_child < nprocs))
		{
			for(i = 0; i < children_result_length; i++)
				result[i+6] = finished_right[i];
		}
	    
	    // Tell parent that the child is done
	    int temp = 1;
	
		//if not the root node then send my data to my parent
		if(id != 0){
			printf("Sending to parent %d from process: %d\n",parent,id);
			MPI_Send(&result, result_length, MPI_LONG_DOUBLE, parent, id, MPI_COMM_WORLD);
		}

		//if process 0 then print the data received
		if(id == 0)
		{		
			/*int temp;
			for (i=2; i<nprocs;i++){
			    MPI_Recv(&temp, 1, MPI_INT, i, i, MPI_COMM_WORLD, &status);
				//printf("Process %d done\n",i);
			}*/

			printf("Result length: %d\n",result_length);
			int level = 1;
			/*for(i = 0; i < result_length; i++)
			{
				printf("Data from level %f\n",floor(log2(level)));
				printf("\tresult: %Lf\n", result[i]);
				printf("\tdelta energy: %Lf\n", result[i+1]);
				printf("\tparticle index removed: %Lf\n", result[i+2]);
				printf("\tnew particle x: : %Lf\n", result[i+3]);
				printf("\tnew particle y: : %Lf\n", result[i+4]);
				printf("\tnew particle z: : %Lf\n", result[i+5]);
				i += 5;
				level=level*2;
			}*/

			
		}
	
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

	//}while();

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

