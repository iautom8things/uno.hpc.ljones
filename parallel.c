#include "pib.h"

void setup_tree (){
    
    if (id != 1 || id != 0){
        int max_buff_size = floor(log2(id))*4; // There's 4 double elements 
        double previous_state[max_buff_size];  
    }
    else if(id==1){
        int max_buff_size = 4;
        double previous_state[1];
    
    if (id==0){
        double temp[] = {-1, -1, -1, -1};
        MPI_Send(temp, 4, MPI_DOUBLE, 1, -1, MPI_COMM_WORLD);
    }
    if (id!=0){
        parent = id/2;
        MPI_Receive(previous_state, max_buff_size, MPI_DOUBLE, parent, MPI_COMM_WORLD);
        
        // update state from passed in previous_state
            //Note: make sure we continue looking after we see a '-1'
            
        // generate information for new particle to perturb
        int index_of_random = (int)rand()%NUMBER_OF_PARTICLES;
        
        double x = SIZE*LENGTH_OF_CUBE*((random_num)/(RAND_MAX+1.0));
        double y = SIZE*LENGTH_OF_CUBE*((random_num)/(RAND_MAX+1.0));
        double z = SIZE*LENGTH_OF_CUBE*((random_num)/(RAND_MAX+1.0));
        
        // copy previous_state and malloc room for new changes, to be sent to children
        double to_be_sent[max_buff_size+4]; 
        
        
        // pass information to children
        int left_child = 2*id;
        int right_child = (2*id)+1;
        
        if (left_child < nprocs)
            //MPI_SEND to left_child
        if (right_child < nprocs)
            //MPI_SEND to right_child
    }
    
}