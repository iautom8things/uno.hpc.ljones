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
	
    int seed; // seed for the random number generator
    char* output_file;
	int starting_trial_amount;

    if (id==0)
        output_file = "data.csv"; // the default output file name

    ///////////   THIS IS FOR YOU DR. SUMMA  //////////////////////
    ///////////         ARGUMENTS            //////////////////////
    
    int i, j;
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
	starting_trial_amount = NUMBER_OF_TRIALS;

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

        calculate_cube_energy(i);
    }
    
    srand(seed+id); // reseed the random number generator so each process has a different seed
    
	//-------Intialise the system energies for process 0------//
	
	int energy_counter = 1;
    int max_number_of_energies = NUMBER_OF_TRIALS + 1;
    
	long double current_system_energy = system_energy(cubes);
    FILE *file;
    if (id==0){
        file = fopen(output_file,"w");
        fprintf(file,"Energy\t%3d\t%Lf\n",0,current_system_energy);
        
    }
	//////---------START MPI STUFF---------///////
	
    int max_buff_size = 4;
    int childrens_max_buff_size = 4;

    if (id != 0) {
        max_buff_size = floor(log2(id+1))*4; // There's 4 double elements 
        childrens_max_buff_size = max_buff_size+4;
    }
    double current_peturbing[4];
    double previous_state[max_buff_size];
    double accepted_state[childrens_max_buff_size]; 
    double rejected_state[childrens_max_buff_size];
    double particle_reversion[childrens_max_buff_size];
    int tree_height = floor(log2(nprocs))+1;
    int children_result_length = 6 * (tree_height-1); // The number of items to be received by 'the rest of the tree', 
															 // bellow the current node. (remaining height*6)
    
    if(id != 0)
        children_result_length -= 6 * floor(log2(id+1)); // If not root, subtract the difference in 
    
    int max_length = tree_height * 6;
    int result_length = children_result_length + 6;
    long double result[result_length];
    long double finished_left[children_result_length];
    long double finished_right[children_result_length];
    int left_child = (2*id)+1;
    int right_child = (2*id)+2;
    int parent = (id-1)/2;
    int live = NUMBER_OF_TRIALS;
    int remaining_trials = NUMBER_OF_TRIALS;
    int count;
    int twentieth;
    
	do{ // While live > 0;
	    
	    if (remaining_trials / tree_height > 0)
            usable_procs = nprocs;
        else
            usable_procs = pow(2,remaining_trials % tree_height)-1;
        //----- Setup the tree so each node picks a particle to move (old, new location) and informs children -----//
	   if (id < usable_procs)
	       setup_tree(max_buff_size, childrens_max_buff_size, previous_state, current_peturbing, accepted_state, rejected_state);
	    
	    //----- Save the information of the particles we remove so we can later revert back -----//
	    if (id != 0){
            for(i = 0; i < max_buff_size; i++) {
		        int index = (int)previous_state[i];
		        particle_reversion[i] = index;
		        particle_reversion[i+1] = particle_array[index].x;
		        particle_reversion[i+2] = particle_array[index].y;
		        particle_reversion[i+3] = particle_array[index].z;
                i+=3;
	        }
        }
        else{
            i = 0;
        }
        int index;
		particle_reversion[i] = index = current_peturbing[0];
		particle_reversion[i+1] = particle_array[index].x;
		particle_reversion[i+2] = particle_array[index].y;
		particle_reversion[i+3] = particle_array[index].z;
		
		//----- Update each nodes particle array and cubes so that the they reflect the state given by the parent node -----//    
	    if(id != 0 && id < usable_procs)
    	    update_state(previous_state, max_buff_size);

	    for(i = 0; i < result_length; i++)
		    result[i] = -1;
	    
	    //----- Each node peturbs a particle: move a particle from old to new location, calculate delta energy, determine result -----//
	    if (id<usable_procs)
	        perturb(current_peturbing, result);	

	    for(i = 0; i < 4; i++)
	        result[i+2] = current_peturbing[i];
 
        //----- Busy wait for children to finish -----//
        if (left_child < usable_procs)
            MPI_Recv(&finished_left, children_result_length, MPI_LONG_DOUBLE, left_child, left_child, MPI_COMM_WORLD, &status);
        if (right_child < usable_procs)
            MPI_Recv(&finished_right, children_result_length, MPI_LONG_DOUBLE, right_child, right_child, MPI_COMM_WORLD, &status);

	    //----- Depending on the result of a node's peturbing, append the appropriate info from children -----//
	    if(result[0] == 1 && (left_child < usable_procs)) { // If the peturbing at a node is SUCCESSFULL, choose the LEFT_CHILD
		    for(i = 0; i < children_result_length; i++)
			    result[i+6] = finished_left[i];
	    }
	    else if(result[0] == 0 && (right_child < usable_procs)) { // If the peturbing at a node is a FAILURE, choose the RIGHT_CHILD
		    for(i = 0; i < children_result_length; i++)
			    result[i+6] = finished_right[i];
		}
		
	    //----- If not the root node then send my data to my parent -----//
	    if(id != 0) {
		    MPI_Send(&result, result_length, MPI_LONG_DOUBLE, parent, id, MPI_COMM_WORLD);
		    MPI_Recv(&live, 1, MPI_INT, 0 , id , MPI_COMM_WORLD, &status);
            remaining_trials = live;

		    //----- Revert and catch up -----//
			int length = max_length - ((max_length/6)*2);
			double catch_up[length];

			//----- Revert -----//		
			for(i = 0; i < max_buff_size+4; i++) {

			    if(particle_reversion[i] != -1) {
				    int cube_index;
				    double x,y,z;
				    int index = (int)particle_reversion[i];

				    remove_particle(&cubes[particle_array[index].myCube], particle_array[index]);

					particle_array[index].x = x = particle_reversion[i+1];
					particle_array[index].y = y = particle_reversion[i+2];
					particle_array[index].z = z = particle_reversion[i+3];
					particle_array[index].myCube = cube_index = belongs_to_cube((int) x/10, (int) y/10, (int) z/10);

				    addToCube(&cubes[cube_index],particle_array[index]);
			    }
			    i += 3;
		    } // END for loop

		    //----- Wait for the catch up state -----//				
		    MPI_Recv(&catch_up, length, MPI_DOUBLE, 0 , id ,MPI_COMM_WORLD, &status);
			//-----UPDATE THE STATE TO CATCH UP WITH PROCESS 0----//
			update_state(catch_up,length);
	    } // END if (id != 0)
        
	    //----- If process 0 then print the data received -----//
	    if(id == 0) {	
		    int level = 1;
            //printf("[ ");
		    for(i = 0; i < result_length; i++) {
		        if (result[i] != -1){ // Skip 'empty' results: This is when we don't have a full tree, and our path leads to a node on the level above the non-complete level
                    current_system_energy += result[i+1];
                    fprintf(file,"Energy\t%3d\t%Lf\n",energy_counter,current_system_energy);
                    //printf("%LF, ", result[i+1]);
				    energy_counter++;
				    level *= 2;
			    }
                    
			    i += 5;
		    } // END for loop
            //printf(" ]\t%d\t%Lf\n", energy_counter,the_system_energy[energy_counter-1]);
            remaining_trials -= log2(level);
            //printf("NUMTRIALS: %d\n", remaining_trials);
            
            live = remaining_trials;
		    int length = max_length - ((max_length/6)*2);
			double parsed_return[length];
			//----- To all processes to continue, send them the information to catch up to root -----//
		    for(i = 1; i < nprocs; i++){
			    MPI_Send(&live, 1, MPI_INT, i, i, MPI_COMM_WORLD);
					
				//-----PARSE THE RESULT DATA FOR THE CHILDREN-----//
				int k = 0, index, cube_index;
                double x, y, z;
				for(j = 2; j < max_length; j++){
					parsed_return[k] = index =(double)result[j];
					parsed_return[k+1] = x =(double)result[j+1];
					parsed_return[k+2] = y =(double)result[j+2];
					parsed_return[k+3] = z =(double)result[j+3];
					if (result[j] != -1){
					    remove_particle(&cubes[particle_array[index].myCube], particle_array[index]);

					    particle_array[index].x = x;
					    particle_array[index].y = y;
					    particle_array[index].z = z;
					    particle_array[index].myCube = cube_index = belongs_to_cube((int) x/10, (int) y/10, (int) z/10);

					    addToCube(&cubes[cube_index],particle_array[index]);  
					}
					k+=4;
					j+=5;						
				}

				//-----SEND THE PARSED DATA------//
			    MPI_Send(&parsed_return, length, MPI_DOUBLE, i, i, MPI_COMM_WORLD);
		    }

            count = NUMBER_OF_TRIALS-remaining_trials;
            twentieth = NUMBER_OF_TRIALS/20;
            printf("\r%7d | %3d%% [",count, (count/twentieth)*5);

            for (j=0;j<count/twentieth;j++){
                printf("--");
            }

            for (j=0;j<(20-count/twentieth);j++){
                printf("  ");
            }
            printf("] 100%%");
            fflush(0);

		} // END if (id == 0)

	}while(live > 0);
	
	//----------------CLOSE THE FILE--------------//
	if (id==0)
		fclose(file);
	
/*
    // Prints the ith particle for every node to check if they are all the same
    i=999;
    printf("%d::%d: x:%f y:%f z:%f\n", id, i, particle_array[i].x,particle_array[i].y,particle_array[i].z);   */ 
    if (id==0)
        printf("\n");
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

