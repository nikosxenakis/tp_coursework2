#ifndef _AFFINITY_SCEDULING_H_
#define _AFFINITY_SCEDULING_H_

#include <stdio.h>

typedef struct {
  int lo;
  int hi;
}Thread_info;

/**
 * @brief Prints the information for the given Thread_info structure
 * @param thread_info Thread_info pointer
 */
extern void print_thread_info(Thread_info* thread_info);

/**
 * @brief Initializes the Thread_Info structure
 * @details Initializes the Thread_Info structure setting the lower and higher position of the chunk that will be executed
 * 
 * @param thread_info Thread_info pointer
 * @param id Thread id
 * @param ipt Initial chunksize per thread
 * @param size Size of all the iterations for the scheduled loop
 */
extern void init_thread_info(Thread_info* thread_info, int id, int ipt, int size);

/**
 * @brief Calculates the remaining load for the given thread
 * 
 * @param thread_info Thread_info pointer
 * @return Returns the remaining load of the given thread
 */
extern int get_thread_info_rem_load(Thread_info* thread_info);

/**
 * @brief Finds the most loaded running thread
 * @details Iterates all the threads in the array and returns the thread with the most remaining load
 * 
 * @param thread_info_array An array that contains Thread_info pointers of all the threads
 * @param size Size of thread_info_array (Number of threads)
 * 
 * @return Returns information for the most loaded thread
 */
extern Thread_info* get_most_loaded_thread_info(Thread_info** thread_info_array, int size);

/**
 * @brief Checks if a thread has finished
 * @details Calculates if the lower equals to the higher position
 * 
 * @param thread_info Thread_info pointer
 * @return Returns 1 if it has finished, 0 if it have not and -1 in error case
 */
extern int finished_thread_info(Thread_info* thread_info);

/**
 * @brief Checks if a thread is available for reducing load
 * 
 * @param thread_info Thread_info pointer
 * @return Returns 1 if it has finished and 0 if it have not
 */
extern int thread_info_is_available(Thread_info* thread_info);

/**
 * @brief Reschedules the chunks of the loaded and finished threads
 * @details Divides in two the chunk of the loaded thread and assigns the latest chunk to the finished thread
 * 
 * @param loaded Thread_info pointer to the most loaded thread
 * @param finished Thread_info pointer to the finished thread
 */
extern void update_chunks(Thread_info* loaded, Thread_info* finished);

#endif /* _AFFINITY_SCEDULING_H_ */
