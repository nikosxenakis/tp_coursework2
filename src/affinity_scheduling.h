//B136013

#ifndef _AFFINITY_SCEDULING_H_
#define _AFFINITY_SCEDULING_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h> 

typedef struct {
  int lo;
  int hi;
} Chunk;

typedef struct {
  omp_lock_t lock;
  int lo;
  int hi;
} Local_Set;

/**
 * @brief Creates a Local_Set structure
 * @param thread_id Thread id
 * @param local_set_size Local Set Size
 * @param iterations Number of iterations in loop
 * @return The new Local_Set
 */
static Local_Set* create_local_set(unsigned int thread_id, unsigned int local_set_size, unsigned int iterations);

/**
 * @brief Calculates the remaining load for the given Local_Set
 * @param local_set Local_Set pointer
 * @return Returns the remaining load of the given Local_Set
 */
static unsigned int get_local_set_rem_load(Local_Set* local_set);

/**
 * @brief Checks if a Local_Set has finished
 * 
 * @param local_set Local_Set pointer
 * @return Returns 1 if it has finished otherwise 0
 */
static int is_finished_local_set(Local_Set* local_set);

/**
 * @brief Calculates the next chunk in the given Local_Set
 * @param local_set Local_Set pointer
 * @param thread_id Thread id
 * @return the new Chunk for execution
 */
static Chunk get_next_chunk_local_set(Local_Set* local_set, unsigned int thread_id);

typedef struct {
  Local_Set** arr;
  unsigned int threds_no;
  unsigned int local_set_size;
} Local_Set_Array;

/**
 * @brief Creates a Local_Set_Array structure
 * @param threds_no Number of threads in loop
 * @param iterations Number of iterations in loop
 * @return The new Local_Set_Array
 */
extern Local_Set_Array* create_local_set_array(unsigned int threds_no, unsigned int iterations);

/**
 * @brief Deallocates the Local_Set_Array memory
 * @param local_set_array The new Local_Set_Array
 */
extern void free_local_set_array(Local_Set_Array* local_set_array);

/**
 * @brief Calculates the next chunk in the given Local_Set_Array
 * @param local_set_array Local_Set_Array pointer
 * @param thread_id Thread id
 * @return the new Chunk for execution
 */
extern Chunk get_next_chunk(Local_Set_Array* local_set_array, unsigned int thread_id);

/**
 * @brief Checks if a loop has finished
 * @param local_set_array Local_Set_Array pointer
 * @return Returns 1 if it has finished otherwise 0
 */
extern int is_finished_loop(Local_Set_Array* local_set_array);
/**
 * @brief Finds the most loaded Local_Set for the given Local_Set_Array
 * @param local_set_array Local_Set_Array pointer
 * @param thread_id Thread id
 * @return Returns the most loaded Local_Set for the given Local_Set_Array
 */
static Local_Set* get_most_loaded_local_set(Local_Set_Array* local_set_array, unsigned int thread_id);

#endif /* _AFFINITY_SCEDULING_H_ */
