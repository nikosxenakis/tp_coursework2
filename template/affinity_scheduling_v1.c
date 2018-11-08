#include "affinity_scheduling.h"

void print_thread_info(Thread_info* thread_info) {
  printf("lo = %d, hi = %d\n", thread_info->lo, thread_info->hi);
}

void init_thread_info(Thread_info* thread_info, int id, int ipt, int size) {
  assert(thread_info && ipt <= size);
  thread_info->lo = id*ipt;
  thread_info->hi = (id+1)*ipt;
  if(thread_info->hi > size)
    thread_info->hi = size;
}

int get_thread_info_rem_load(Thread_info* thread_info) {
  assert(thread_info);
  return thread_info->hi - thread_info->lo;
}

Thread_info* get_most_loaded_thread_info(Thread_info** thread_info_array, int size) {
  Thread_info* thread_info = NULL;
  for (int i = 0; i < size; ++i) {
    if(thread_info == NULL) {
        thread_info = thread_info_array[i];
    }
    else {
      if( get_thread_info_rem_load(thread_info_array[i]) > get_thread_info_rem_load(thread_info)) {
        thread_info = thread_info_array[i];
      }
    }
  }

  return thread_info;
}

int finished_thread_info(Thread_info* thread_info) {
  if(thread_info->lo == thread_info->hi)
    return 1;
  else if(thread_info->lo - 1 < thread_info->hi)
    return 0;
  return -1;
}

int thread_info_is_available(Thread_info* thread_info) {
  if(!finished_thread_info(thread_info) && thread_info->lo + 1 < thread_info->hi) {
    return 1;
  }
  return 0;
}

void update_chunks(Thread_info* loaded, Thread_info* finished) {
  assert(loaded && finished);
  if(thread_info_is_available(loaded)) {
    int new_lo = loaded->lo + ((loaded->hi - loaded->lo)/2);
    finished->lo = new_lo;
    finished->hi = loaded->hi;
    loaded->hi = new_lo;
  }
}
