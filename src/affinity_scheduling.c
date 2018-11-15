#include "affinity_scheduling.h"

Local_Set* create_local_set(unsigned int thread_id, unsigned int local_set_size, unsigned int iterations) {
  Local_Set* local_set = (Local_Set*)malloc(sizeof(Local_Set));
  omp_init_lock(&(local_set->lock));
  local_set->lo = thread_id*local_set_size;
  local_set->hi = (thread_id+1)*local_set_size;
  if(local_set->hi > iterations) local_set->hi = iterations;
  return local_set;
}

void print_local_set(Local_Set* local_set) {
  assert(local_set);
	printf("\tLocal Set: lo = %d, hi = %d\n", local_set->lo, local_set->hi);
}

Local_Set_Array* create_local_set_array(unsigned int threds_no, unsigned int iterations) {
  Local_Set_Array* local_set_array = (Local_Set_Array*)malloc(sizeof(Local_Set_Array));

  local_set_array->arr = (Local_Set**)malloc(threds_no*sizeof(Local_Set*));
  local_set_array->threds_no = threds_no;
  local_set_array->local_set_size = (int) ceil((double)iterations/(double)threds_no);

  for (int thread_id = 0; thread_id < local_set_array->threds_no; ++thread_id) {
    Local_Set* local_set = create_local_set(thread_id, local_set_array->local_set_size, iterations);
    local_set_array->arr[thread_id] = local_set;
  }

  return local_set_array;
}

unsigned int get_local_set_rem_load(Local_Set* local_set) {
  assert(local_set);
	return local_set->hi - local_set->lo;
}

int is_finished_local_set(Local_Set* local_set) {
  assert(local_set);

  return local_set->hi == local_set->lo;
}

Chunk get_next_chunk_local_set(Local_Set* local_set, unsigned int threds_no) {
  assert(local_set);

  Chunk next_chunk;
  unsigned int chunksize, rem_load;

  rem_load = local_set->hi - local_set->lo;

  chunksize = (rem_load > threds_no) ? rem_load / threds_no : 1;

  next_chunk.lo = local_set->lo;

  local_set->lo += chunksize;

  next_chunk.hi = local_set->lo;

  return next_chunk;
}

void free_local_set_array(Local_Set_Array* local_set_array) {
  assert(local_set_array);

  for (int thread_id = 0; thread_id < local_set_array->threds_no; ++thread_id) {
    Local_Set* local_set = local_set_array->arr[thread_id];
    omp_destroy_lock(&(local_set->lock));
  	free (local_set);
  }
  free (local_set_array->arr);
  free (local_set_array);
}

Chunk steal_next_chunk(Local_Set_Array* local_set_array, unsigned int thread_id) {
  Local_Set* local_set = NULL;
  Chunk next_chunk = {0, 0};

  local_set = get_most_loaded_local_set(local_set_array, thread_id);
  if(local_set) {
    next_chunk = get_next_chunk_local_set(local_set, local_set_array->threds_no);
    omp_unset_lock(&(local_set->lock));
  }

  return next_chunk;
}

Chunk get_next_chunk(Local_Set_Array* local_set_array, unsigned int thread_id) {
  assert(local_set_array);

  Local_Set* local_set = local_set_array->arr[thread_id];
  Chunk next_chunk = {0, 0};

  if(is_finished_local_set(local_set)) {
    next_chunk = steal_next_chunk(local_set_array, thread_id);
  }
  else {
    omp_set_lock(&(local_set->lock));

    if(!is_finished_local_set(local_set)) {
      next_chunk = get_next_chunk_local_set(local_set, local_set_array->threds_no);
    }
    omp_unset_lock(&(local_set->lock));
  }

  return next_chunk;
}

int is_finished_loop(Local_Set_Array* local_set_array) {
  assert(local_set_array);

  for (int thread_id = 0; thread_id < local_set_array->threds_no; ++thread_id) {
    if(!is_finished_local_set(local_set_array->arr[thread_id]))
      return 0;
  }
  return 1;
}

Local_Set* get_most_loaded_local_set(Local_Set_Array* local_set_array, unsigned int curr_thread_id) {
  assert(local_set_array);

  Local_Set *loaded_local_set = NULL, *tmp_local_set = NULL;
  unsigned int max_load = 0, tmp_load = 0;  

  for (int thread_id = 0; thread_id < local_set_array->threds_no; ++thread_id) {

    tmp_local_set = local_set_array->arr[thread_id];

    tmp_load = get_local_set_rem_load(tmp_local_set);

    if(tmp_load <= max_load)
      continue;

    omp_set_lock(&(tmp_local_set->lock));

    tmp_load = get_local_set_rem_load(tmp_local_set);

    if(tmp_load > max_load) {
      if(loaded_local_set && loaded_local_set != tmp_local_set)
        omp_unset_lock(&(loaded_local_set->lock));
      max_load = tmp_load;
      loaded_local_set = tmp_local_set;
    }
    else {
      omp_unset_lock(&(tmp_local_set->lock));
    }

  }

  return loaded_local_set;
}
