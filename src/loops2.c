#include <stdio.h>
#include <math.h>


#define N 729
#define reps 1000
#include <omp.h>

#define TIMES 20

#include "affinity_scheduling.h"

double a[N][N], b[N][N], c[N];
int jmax[N];


void init1(void);
void init2(void);
void runloop(int);
void loop1chunk(int, int);
void loop2chunk(int, int);
void valid1(void);
void valid2(void);


int main(int argc, char *argv[]) {

  double start1,start2,end1,end2,mean1,mean2;
  int r;

  for (int i = 0; i < TIMES; ++i) {
    init1();

    start1 = omp_get_wtime();

    for (r=0; r<reps; r++){
      runloop(1);
    }

    end1  = omp_get_wtime();
    mean1 += end1 - start1;
  }
  mean1 /= TIMES;

  valid1();

  printf("Total time for %d reps of loop 1 = %f\n",reps, (float)(mean1));

  for (int i = 0; i < TIMES; ++i) {
    init2();

    start2 = omp_get_wtime();

    for (r=0; r<reps; r++){
      runloop(2);
    }

    end2  = omp_get_wtime();
    mean2 += end2 - start2;
  }
  mean2 /= TIMES;

  valid2();

  printf("Total time for %d reps of loop 2 = %f\n",reps, (float)(mean2));

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

Local_Set_Array* local_set_array = NULL;

#pragma omp parallel default(none) shared(loopid, local_set_array)
  {
    int myid  = omp_get_thread_num();
    int nthreads = omp_get_num_threads();
    Chunk next_chunk = {0, 0};

    #pragma omp single
    {
      local_set_array = create_local_set_array(nthreads, N);
    }
    #pragma omp barrier

    while(!is_finished_loop(local_set_array)) {

      next_chunk = get_next_chunk(local_set_array, myid);

      switch (loopid) {
        case 1: loop1chunk(next_chunk.lo,next_chunk.hi); break;
        case 2: loop2chunk(next_chunk.lo,next_chunk.hi); break;
      }

    }

    #pragma omp barrier
    #pragma omp single
    {
      free_local_set_array(local_set_array);
    }

  }
}

void loop1chunk(int lo, int hi) {
  int i,j;

  for (i=lo; i<hi; i++){
    for (j=N-1; j>i; j--){
      a[i][j] += cos(b[i][j]);
    }
  }

}



void loop2chunk(int lo, int hi) {
  int i,j,k;
  double rN2;

  rN2 = 1.0 / (double) (N*N);

  for (i=lo; i<hi; i++){
    for (j=0; j < jmax[i]; j++){
      for (k=0; k<j; k++){
  c[i] += (k+1) * log (b[i][j]) * rN2;
      }
    }
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

}


void valid2(void) {
  int i;
  double sumc;

  sumc= 0.0;
  for (i=0; i<N; i++){
    sumc += c[i];
  }
  printf("Loop 2 check: Sum of c is %f\n", sumc);
}
