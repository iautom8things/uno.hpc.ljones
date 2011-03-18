#include "pib.h"

void setup_tree (int max_buff_size, int childrens_max_buff_size, double *previous_state, double *current_peturbing, double *accepted_state, double *rejected_state){
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
        // Busy wait for parent's state
        MPI_Recv(previous_state, max_buff_size, MPI_DOUBLE, parent, parent, MPI_COMM_WORLD, &status);
        
        // Debugging print lines
        /*printf("ID:%d\n{ ", id);
        for (i=0; i<max_buff_size;i++){
            printf("%f, ",previous_state[i]);
        }
        printf(" }\n");*/
        // End debugging print lines
         
        // generate information for new particle to perturb
        current_peturbing[0] = (int)rand()%NUMBER_OF_PARTICLES;    // Index
        current_peturbing[1] = SIZE*LENGTH_OF_CUBE*((rand())/(RAND_MAX+1.0)); // x coordinate
        current_peturbing[2] = SIZE*LENGTH_OF_CUBE*((rand())/(RAND_MAX+1.0)); // y coordinate
        current_peturbing[3] = SIZE*LENGTH_OF_CUBE*((rand())/(RAND_MAX+1.0)); // z coordinate
		
        
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

		/* We need to do our work in between the send and the recieve 

        // busy wait for children to finish
        if (left_child < nprocs)
            MPI_Recv(&finished, 1, MPI_INT, left_child, left_child, MPI_COMM_WORLD, &status);
        if (right_child < nprocs)
            MPI_Recv(&finished, 1, MPI_INT, right_child, right_child, MPI_COMM_WORLD, &status);
        
        // Tell parent that the child is done
        int temp = 1;
        MPI_Send(&temp, 1, MPI_INT, parent, id, MPI_COMM_WORLD);

		*/
    } // End if not main process  
}
void update_state(cube *cubes, particle *particle_array, double *previous_state, double *current_peturbing, int max_buff_size){
    //adjust array to parents state
    int particle_index, i, j, cube_index;
    double x, y, z;
	for (i=0; i<max_buff_size;i++){
        if (previous_state[i] != -1){
            particle_index = (int) previous_state[i];
            cube_index = particle_array[particle_index].myCube;
            
       /*     ////// Debugging print lines
            printf("%d: Cube:%d #:%d\n", id, cube_index, cubes[cube_index].number_of_particles);
            for (j=0; j<cubes[cube_index].number_of_particles; j++)
                printf("%d: x: %f y: %f z: %f\n", id, cubes[cube_index].particles[j].x, cubes[cube_index].particles[j].y, cubes[cube_index].particles[j].z);
			////// End Debugging print lines
		*/	
			remove_particle(&cubes[cube_index], particle_array[particle_index]);
			
		/*	////// Debugging print lines
            printf("%d: Cube:%d #:%d\n", id, cube_index, cubes[cube_index].number_of_particles);
            for (j=0; j<cubes[cube_index].number_of_particles; j++)
                printf("%d: x: %f y: %f z: %f\n", id, cubes[cube_index].particles[j].x, cubes[cube_index].particles[j].y, cubes[cube_index].particles[j].z);
          */  ////// End Debugging print lines
            
			particle_array[particle_index].x = x = previous_state[i+1];
			particle_array[particle_index].y = y = previous_state[i+2];
			particle_array[particle_index].z = z = previous_state[i+3];
			particle_array[particle_index].myCube = cube_index = belongs_to_cube((int) x/10,
													                            (int) y/10,
															                    (int) z/10);

		/*	////// Debugging print lines
            printf("%d: Cube:%d #:%d\n", id, cube_index, cubes[cube_index].number_of_particles);
            for (j=0; j<cubes[cube_index].number_of_particles; j++)
                printf("%d: x: %f y: %f z: %f\n", id, cubes[cube_index].particles[j].x, cubes[cube_index].particles[j].y, cubes[cube_index].particles[j].z);
         */   ////// End Debugging print lines

			addToCube(&cubes[cube_index],particle_array[particle_index]);

		/*	////// Debugging print lines
            printf("%d: Cube:%d #:%d\n", id, cube_index, cubes[cube_index].number_of_particles);
            for (j=0; j<cubes[cube_index].number_of_particles; j++)
                printf("%d: x: %f y: %f z: %f\n", id, cubes[cube_index].particles[j].x, cubes[cube_index].particles[j].y, cubes[cube_index].particles[j].z);
          */  ////// End Debugging print lines
            i += 3;
		} // end not -1
		else
            i += 3;
    }
    // update the particle array with the information of the current state's particle peturbing
    
}

void perturb(double *current_perturbing, long double * result){
    int i;	
	double x,y,z;

	int particle_index = (int)current_perturbing[0];
	particle_array[particle_index].x = x = current_perturbing[1];
	particle_array[particle_index].y = y = current_perturbing[2];
	particle_array[particle_index].z = z = current_perturbing[3];
    particle_array[particle_index].myCube = belongs_to_cube((int) x/10, (int) y/10, (int) z/10);    
   

	long double delta = delta_energy(particle_index,particle_array[particle_index]);

	result[1] = delta;

	//compare energy
	double probability = compare_energies(delta);
    
    //if not acceptable then revert the changes
	//use a old_particle acceptance level
	//MONTE CARLO THAT THANG
	if( ((rand())/(RAND_MAX+1.0)) > probability)
		result[0] = 0;
	else
		result[0] = 1;

	//we should send a two element paylod to the parent
	//1st element is the accpetance (true or flase)
	//2nd the delta energy, to be collected
    
}
