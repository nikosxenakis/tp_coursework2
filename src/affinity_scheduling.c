#include "affinity_scheduling.h"

// Creates a Local_Set structure
Local_Set* create_local_set(unsigned int thread_id, unsigned int local_set_size, unsigned int iterations) {
  Local_Set* local_set = (Local_Set*)malloc(sizeof(Local_Set));
  //initializes the omp_lock_t
  omp_init_lock(&(local_set->lock));
  local_set->lo = thread_id*local_set_size;
  local_set->hi = (thread_id+1)*local_set_size;
  //sets the correct hi position to the last thread
  if(local_set->hi > iterations) local_set->hi = iterations;
  return local_set;
}

//Calculates the remaining load for the given Local_Set which is the different between hi and lo
unsigned int get_local_set_rem_load(Local_Set* local_set) {
  return local_set->hi - local_set->lo;
}

//Checks if a Local_Set has finished this is true if the hi equals to the lo position
int is_finished_local_set(Local_Set* local_set) {
  return local_set->hi == local_set->lo;
}

//Calculates the next chunk in the given Local_Set
Chunk get_next_chunk_local_set(Local_Set* local_set, unsigned int threds_no) {
  Chunk next_chunk;
  unsigned int chunksize, rem_load;

  //finds the remaining load
  rem_load = get_local_set_rem_load(local_set);

  //calculates the next chunksize based on the specifications
  chunksize = (rem_load > threds_no) ? rem_load / threds_no : 1;

  //updates the new lo and hi
  next_chunk.lo = local_set->lo;
  local_set->lo += chunksize;
  next_chunk.hi = local_set->lo;

  return next_chunk;
}

//Creates a Local_Set_Array structure
Local_Set_Array* create_local_set_array(unsigned int threds_no, unsigned int iterations) {
  Local_Set_Array* local_set_array = (Local_Set_Array*)malloc(sizeof(Local_Set_Array));

  //allocates Local Sets for each thread
  local_set_array->arr = (Local_Set**)malloc(threds_no*sizeof(Local_Set*));
  local_set_array->threds_no = threds_no;
  local_set_array->local_set_size = (int) ceil((double)iterations/(double)threds_no);

  //initializes the new Local Sets and stores them to the array
  for (int thread_id = 0; thread_id < local_set_array->threds_no; ++thread_id) {
    Local_Set* local_set = create_local_set(thread_id, local_set_array->local_set_size, iterations);
    local_set_array->arr[thread_id] = local_set;
  }

  return local_set_array;
}

//Deallocates the Local_Set_Array memory
void free_local_set_array(Local_Set_Array* local_set_array) {
  for (int thread_id = 0; thread_id < local_set_array->threds_no; ++thread_id) {
    Local_Set* local_set = local_set_array->arr[thread_id];
    //destroys the omp_lock_t for each Local Set
    omp_destroy_lock(&(local_set->lock));
  	free (local_set);
  }
  free (local_set_array->arr);
  free (local_set_array);
}

//Steals the next Chunk from the most loaded thread
Chunk steal_next_chunk(Local_Set_Array* local_set_array, unsigned int thread_id) {
  Local_Set* local_set = NULL;
  Chunk next_chunk = {0, 0};

  //finds the most loaded Local Set
  local_set = get_most_loaded_local_set(local_set_array, thread_id);
  if(local_set) {

    //Gets the next Chunk from the most loaded Local Set
    next_chunk = get_next_chunk_local_set(local_set, local_set_array->threds_no);

    //unlocks the most loaded Local Set
    //this Local Set has been locked from the get_next_chunk_local_set function
    omp_unset_lock(&(local_set->lock));
  }

  return next_chunk;
}

//Calculates the next chunk in the given Local_Set_Array
Chunk get_next_chunk(Local_Set_Array* local_set_array, unsigned int thread_id) {
  Local_Set* local_set = local_set_array->arr[thread_id];
  Chunk next_chunk = {0, 0};

  //if its assigned Local Set is finished steals the next Chunk from the most loaded thread
  if(is_finished_local_set(local_set)) {
    next_chunk = steal_next_chunk(local_set_array, thread_id);
  }
  else {
    //tries to lock its Local Set if anyone attemps to steal from this thread
    omp_set_lock(&(local_set->lock));

    //if it is not finished it gets the next Chunk from its Local Set
    if(!is_finished_local_set(local_set)) {
      next_chunk = get_next_chunk_local_set(local_set, local_set_array->threds_no);
    }

    //unlocks the Local Set
    omp_unset_lock(&(local_set->lock));
  }

  return next_chunk;
}

//Checks if a loop has finished
int is_finished_loop(Local_Set_Array* local_set_array) {
  //checks for each Local Set if all of them are finished
  for (int thread_id = 0; thread_id < local_set_array->threds_no; ++thread_id) {
    if(!is_finished_local_set(local_set_array->arr[thread_id]))
      return 0;
  }
  return 1;
}

//Finds the most loaded Local_Set for the given Local_Set_Array
Local_Set* get_most_loaded_local_set(Local_Set_Array* local_set_array, unsigned int curr_thread_id) {
  Local_Set *loaded_local_set = NULL, *tmp_local_set = NULL;
  unsigned int max_load = 0, tmp_load = 0;  

  //Iterates all of the Local Sets
  for (int thread_id = 0; thread_id < local_set_array->threds_no; ++thread_id) {

    tmp_local_set = local_set_array->arr[thread_id];

    //If this Local Set has bigger load it tries to lock it
    omp_set_lock(&(tmp_local_set->lock));

    //Takes again its load (in case the previous thread has updated it)
    tmp_load = get_local_set_rem_load(tmp_local_set);

    if(tmp_load > max_load) {
      //If we have a most loaded Local Set that needs to be replaced we unlock it for the other threads
      if(loaded_local_set && loaded_local_set != tmp_local_set)
        omp_unset_lock(&(loaded_local_set->lock));

      //We update the maximum load and new most loaded Local Set
      max_load = tmp_load;
      loaded_local_set = tmp_local_set;
    }
    else {
      //if the temporary Local Set has changes and it is not the most loaded we unlock it
      omp_unset_lock(&(tmp_local_set->lock));
    }

  }

  return loaded_local_set;
}
