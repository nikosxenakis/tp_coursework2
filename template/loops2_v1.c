#include <stdio.h>
#include <math.h>
#include <assert.h>

#define N 729
#define reps 1000
#include <omp.h> 
#include "affinity_scheduling.h"

double a[N][N], b[N][N], c[N];
int jmax[N];  

void init1(void);
void init2(void);
void runloop(int); 
void loop1chunk(Thread_info* thread_info);
void loop2chunk(Thread_info* thread_info);
void valid1(void);
void valid2(void);


int main(int argc, char *argv[]) { 

  double start1,start2,end1,end2;
  int r;

  init1(); 

  start1 = omp_get_wtime(); 

  for (r=0; r<reps; r++){ 
    runloop(1);
  } 

  end1  = omp_get_wtime();  

  valid1(); 

  printf("Total time for %d reps of loop 1 = %f\n",reps, (float)(end1-start1)); 

  init2(); 

  start2 = omp_get_wtime(); 

  for (r=0; r<reps; r++){ 
    runloop(2);
  } 

  end2  = omp_get_wtime(); 

  valid2(); 

  printf("Total time for %d reps of loop 2 = %f\n",reps, (float)(end2-start2)); 

} 

void init1(void){
  int i,j; 

  for (i=0; i<N; i++){ 
    for (j=0; j<N; j++){ 
      a[i][j] = 0.0; 
      b[i][j] = 3.142*(i+j); 
    }
  }

}

void init2(void){ 
  int i,j, expr; 

  for (i=0; i<N; i++){ 
    expr =  i%( 3*(i/30) + 1); 
    if ( expr == 0) { 
      jmax[i] = N;
    }
    else {
      jmax[i] = 1; 
    }
    c[i] = 0.0;
  }

  for (i=0; i<N; i++){ 
    for (j=0; j<N; j++){ 
      b[i][j] = (double) (i*j+1) / (double) (N*N); 
    }
  }
 
} 

void runloop(int loopid)  {

Thread_info** thread_info_array;

#pragma omp parallel default(none) shared(loopid, thread_info_array) 
  {
    int nthreads = omp_get_num_threads();
    int id = omp_get_thread_num();
    int ipt = (int) ceil((double)N/(double)nthreads); 

    #pragma omp master
    {
      thread_info_array = (Thread_info**)malloc(nthreads*sizeof(Thread_info*));
      for (int i = 0; i < nthreads; ++i) {
        thread_info_array[i] = (Thread_info*)malloc(sizeof(Thread_info));
      }
    }
    #pragma omp barrier

    init_thread_info(thread_info_array[id], id, ipt, N);

    while(!finished_thread_info(thread_info_array[id])) {
      switch (loopid) { 
       case 1: loop1chunk(thread_info_array[id]); break;
       case 2: loop2chunk(thread_info_array[id]); break;
      }


      Thread_info* most_loaded_thread_info = get_most_loaded_thread_info(thread_info_array, nthreads);
      #pragma omp critical
      update_chunks(most_loaded_thread_info, thread_info_array[id]);

    }

    #pragma omp barrier
    #pragma omp master
    {
      for (int i = 0; i < nthreads; ++i) {
        free (thread_info_array[i]);
      }
      free (thread_info_array);
    }
  }
}

void loop1chunk(Thread_info* thread_info) { 
  int i,j; 
  
  for (i=thread_info->lo; i<thread_info->hi; i++){ 
    for (j=N-1; j>i; j--){
      a[i][j] += cos(b[i][j]);
    } 

    #pragma omp critical
    thread_info->lo++;
  }

} 

void loop2chunk(Thread_info* thread_info) {
  int i,j,k; 
  double rN2; 

  rN2 = 1.0 / (double) (N*N);  

  for (i=thread_info->lo; i<thread_info->hi; i++){ 
    for (j=0; j < jmax[i]; j++){
      for (k=0; k<j; k++){ 
        c[i] += (k+1) * log (b[i][j]) * rN2;
      } 
    }
  
    #pragma omp critical
    thread_info->lo++;
  }

}

void valid1(void) { 
  int i,j; 
  double suma; 
  
  suma= 0.0; 
  for (i=0; i<N; i++){ 
    for (j=0; j<N; j++){ 
      suma += a[i][j];
    }
  }
  printf("Loop 1 check: Sum of a is %lf\n", suma);
  assert(suma - (-343021.474766) < 0.000001);  
} 


void valid2(void) { 
  int i; 
  double sumc; 
  
  sumc= 0.0; 
  for (i=0; i<N; i++){ 
    sumc += c[i];
  }
  printf("Loop 2 check: Sum of c is %f\n", sumc);
  assert(sumc - (-25242644.603147) < 0.000001);
} 
 
