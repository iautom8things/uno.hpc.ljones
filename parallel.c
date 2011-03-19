#include "pib.h"

void setup_tree (int max_buff_size, int childrens_max_buff_size, double *previous_state, double *current_peturbing, double *accepted_state, double *rejected_state){
    MPI_Status status;
    
    int finished = 0, i, j, parent = (id-1)/2;
		
	if (id!=0){
		//printf("Process %d waiting for parent %d\n",id,parent);		    
		// Busy wait for parent's state
	    MPI_Recv(previous_state, max_buff_size, MPI_DOUBLE, parent, parent, MPI_COMM_WORLD, &status);
	}

    // generate information for new particle to perturb
    current_peturbing[0] = (int)rand()%NUMBER_OF_PARTICLES;    // Index
    current_peturbing[1] = SIZE*LENGTH_OF_CUBE*((rand())/(RAND_MAX+1.0)); // x coordinate
    current_peturbing[2] = SIZE*LENGTH_OF_CUBE*((rand())/(RAND_MAX+1.0)); // y coordinate
    current_peturbing[3] = SIZE*LENGTH_OF_CUBE*((rand())/(RAND_MAX+1.0)); // z coordinate
	
    
    // copy previous_state (from parent), to be sent to children
    if (id != 0){ // node 1 doesn't do this because it's sent a 'dummy previous state' from 0 because node 1 already has the current state
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
    int left_child = (2*id)+1;
    int right_child = (2*id)+2;
     
    if (left_child < (nprocs)){
        //printf("My buff: %d\tChild buff: %d\tleft_child: %d\t right: %d\n", max_buff_size, childrens_max_buff_size, left_child, right_child);
        MPI_Send(accepted_state, childrens_max_buff_size, MPI_DOUBLE, left_child, id, MPI_COMM_WORLD);
    }
    if (right_child < (nprocs)){
        //printf("My buff: %d\tChild buff: %d\tleft_child: %d\t right: %d\n", max_buff_size, childrens_max_buff_size, left_child, right_child);
        MPI_Send(rejected_state, childrens_max_buff_size, MPI_DOUBLE, right_child, id, MPI_COMM_WORLD);
    }
}


void update_state(cube *cubes, particle *particle_array, double *previous_state, double *current_peturbing, int max_buff_size){

    //adjust array to parents state
    int particle_index, i, j, cube_index;
    double x, y, z;
	for (i=0; i<max_buff_size;i++){
        if (previous_state[i] != -1){
            particle_index = (int) previous_state[i];
            cube_index = particle_array[particle_index].myCube;
		
			remove_particle(&cubes[cube_index], particle_array[particle_index]);
            
			particle_array[particle_index].x = x = previous_state[i+1];
			particle_array[particle_index].y = y = previous_state[i+2];
			particle_array[particle_index].z = z = previous_state[i+3];
			particle_array[particle_index].myCube = cube_index = belongs_to_cube((int) x/10,
													                            (int) y/10,
															                    (int) z/10);

			addToCube(&cubes[cube_index],particle_array[particle_index]);

            i += 3;
		} // end not -1
		else
            i += 3;
    }
    
}

void perturb(double *current_perturbing, long double * result){
    int i;	
	double x,y,z;

	int particle_index = (int)current_perturbing[0];
	particle new_particle;
	new_particle.x = x = current_perturbing[1];
	new_particle.y = y = current_perturbing[2];
	new_particle.z = z = current_perturbing[3];
    new_particle.myCube = belongs_to_cube((int) x/10, (int) y/10, (int) z/10);    
   

	long double delta = delta_energy(particle_index,new_particle);

	result[1] = delta;

	//compare energy
	double probability = compare_energies(delta);
    
    //if not acceptable then revert the changes
	//use a old_particle acceptance level
	//MONTE CARLO THAT THANG
	if( ((rand())/(RAND_MAX+1.0)) < probability)
		result[0] = 0;
	else
		result[0] = 1;

	//we should send a two element paylod to the parent
	//1st element is the accpetance (true or flase)
	//2nd the delta energy, to be collected
    
}
