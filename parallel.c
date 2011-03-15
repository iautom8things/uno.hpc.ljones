#include "pib.h"

void setup_tree (){
    srand(0); // seed the random number generator
    double random_num = rand(); //get a random number
    MPI_Status status;
    
    int max_buff_size = 4;
    double previous_state[1];
    
    if (id != 1 || id != 0){
        int max_buff_size = floor(log2(id))*4; // There's 4 double elements 
        double previous_state[max_buff_size];  
    }
    if (id==0){
        double temp[] = {-1, -1, -1, -1};
        MPI_Send(temp, 4, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
    }
    if (id!=0){
        int parent = id/2;
        MPI_Recv(previous_state, max_buff_size, MPI_DOUBLE, parent, parent, MPI_COMM_WORLD, &status);
        
        // update state from passed in previous_state
            //Note: make sure we continue looking after we see a '-1'
            
        // generate information for new particle to perturb
        int index_of_random = (int)rand()%NUMBER_OF_PARTICLES;
        
        double x = SIZE*LENGTH_OF_CUBE*((random_num)/(RAND_MAX+1.0));
        double y = SIZE*LENGTH_OF_CUBE*((random_num)/(RAND_MAX+1.0));
        double z = SIZE*LENGTH_OF_CUBE*((random_num)/(RAND_MAX+1.0));
        
        // copy previous_state and information from current state, to be sent to children
        int childrens_max_buff_size = max_buff_size+4;
        double accepted_state[childrens_max_buff_size]; 
        double rejected_state[childrens_max_buff_size];
        
        int i;
        
        // The following can be condensed by combining the 'genereate info' portion with this
        // Written explicitly at first so as to not get ahead of myself
        
        for (i=0;i<max_buff_size; i++)
            accepted_state[i] = rejected_state[i] = previous_state[i];
            
        accepted_state[i] = index_of_random;
        rejected_state[i++] = -1;
        accepted_state[i] = x;
        rejected_state[i++] = -1;
        accepted_state[i] = y;
        rejected_state[i++] = -1;
        accepted_state[i] = z;
        rejected_state[i] = -1;
        
        // pass information to children
        int left_child = 2*id;
        int right_child = (2*id)+1;
        printf("My buff: %d\tChild buff: %d\tleft_child: %d\t right: %d\n", max_buff_size, childrens_max_buff_size, left_child, right_child);
        if (left_child < nprocs)
            MPI_Send(accepted_state, childrens_max_buff_size, MPI_DOUBLE, left_child, id, MPI_COMM_WORLD);
        if (right_child < nprocs)
            MPI_Send(rejected_state, childrens_max_buff_size, MPI_DOUBLE, right_child, id, MPI_COMM_WORLD);
    }
    
}