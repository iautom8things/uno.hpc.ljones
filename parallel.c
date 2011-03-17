#include "pib.h"

void setup_tree (int max_buff_size, int childrens_max_buff_size, double *previous_state, double *accepted_state, double *rejected_state){
    MPI_Status status;
    
    int finished = 0, i, j, parent = id/2;
 
    if (id==0){
        // dummy info being sent to 1 to start
        // TODO try to send nothing or just 1 int. Really this would require a special case to be written for node 1. May be worth leaving as-is
        double temp[] = {-1, -1, -1, -1};
        // Start the tree setup and busy wait for the entire tree to be finished setting up
        MPI_Sendrecv(&temp, 4, MPI_DOUBLE, 1, 0, &finished, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status); 
    }
    if (id!=0){
        
        MPI_Recv(previous_state, max_buff_size, MPI_DOUBLE, parent, parent, MPI_COMM_WORLD, &status);
        printf("ID:%d\n{ ", id);
        for (i=0; i<max_buff_size;i++){
            printf("%f, ",previous_state[i]);
        }
        printf(" }\n");
        // update state from passed in previous_state
            //Note: make sure we continue looking after we see a '-1'
            
        // generate information for new particle to perturb
        //int index_of_random = (int)rand()%NUMBER_OF_PARTICLES;
        double current_peturbing[] = { (int)rand()%NUMBER_OF_PARTICLES, // Index
                                        SIZE*LENGTH_OF_CUBE*((rand())/(RAND_MAX+1.0)), // x coordinate
                                        SIZE*LENGTH_OF_CUBE*((rand())/(RAND_MAX+1.0)), // y coordinate
                                        SIZE*LENGTH_OF_CUBE*((rand())/(RAND_MAX+1.0))};// z coordinate
		
        
        // copy previous_state (from parent), to be sent to children
        if (id != 1){ // node 1 doesn't do this because it's sent a 'dummy previous state' from 0 because node 1 already has the current state
            for (i=0;i<max_buff_size; i++){
                accepted_state[i] = previous_state[i];
                rejected_state[i] = previous_state[i];
            }
        }
        else
            i=0;
        
        // copy the current peturbing to the accepted state, and send -1s to rejected state, notifying a rejection
        for (j=0; j < 4; j++, i++){
            accepted_state[i] = current_peturbing[j];
            rejected_state[i] = -1;
        }

        // pass information to children
        int left_child = 2*id;
        int right_child = (2*id)+1;
         
        if (left_child < nprocs){
            //printf("My buff: %d\tChild buff: %d\tleft_child: %d\t right: %d\n", max_buff_size, childrens_max_buff_size, left_child, right_child);
            MPI_Send(accepted_state, childrens_max_buff_size, MPI_DOUBLE, left_child, id, MPI_COMM_WORLD);
        }
        if (right_child < nprocs){
            //printf("My buff: %d\tChild buff: %d\tleft_child: %d\t right: %d\n", max_buff_size, childrens_max_buff_size, left_child, right_child);
            MPI_Send(rejected_state, childrens_max_buff_size, MPI_DOUBLE, right_child, id, MPI_COMM_WORLD);
        }


		//adjust array to parents state
		for (i=0; i<max_buff_size;i++){
            if(i == -1)
				i += 4;
			else
			{
				remove_particle(&cubes, particle_array[(int)previous_state[i]);
				particle_array[(int)previous_state[i]].x = previous_state[i+1];
				particle_array[(int)previous_state[i]].y = previous_state[i+2];
				particle_array[(int)previous_state[i]].z = previous_state[i+1];
				particle_array[(int)previous_state[i]].myCube = belongs_to_cube((int) previous_state[i+1]/10,
																				(int) previous_state[i+2]/10,
																				(int) previous_state[i+3]/10);
				addToCube(&cubes,particle_array[(int)previous_state[i]]);
			}
        }

		particle_array[current_peturbing[0]].x = current_peturbing[1];
		particle_array[current_peturbing[0]].y = current_peturbing[2];
		particle_array[current_peturbing[0]].z = current_peturbing[3];

		//perturb
		long double delta = 0;//perturb();

		//compare energy
		double probability = compare_energies(delta);

		//if not acceptable then revert the changes
		//use a old_particle acceptance level
		//MONTE CARLO THAT THANG
		if( ((rand())/(RAND_MAX+1.0)) > probability)
		{
		   
		    //fail
		    //delta_energy = 0for (i=0; i<max_buff_size;i++){
            printf("%f, ",previous_state[i]);
        }

		//we should send a two element paylod to the parent
		//1st element is the accpetance (true or flase)
		//2nd the delta energy, to be collected
		
		
	
        // busy wait for children to finish
        if (left_child < nprocs)
            MPI_Recv(&finished, 1, MPI_INT, left_child, left_child, MPI_COMM_WORLD, &status);
        if (right_child < nprocs)
            MPI_Recv(&finished, 1, MPI_INT, right_child, right_child, MPI_COMM_WORLD, &status);
        
        // Tell parent that the child is done
        int temp = 1;
        MPI_Send(&temp, 1, MPI_INT, parent, id, MPI_COMM_WORLD);
    }
    
}
