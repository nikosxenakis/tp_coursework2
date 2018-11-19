
# Threaded Programming Coursework Part 2

## Building

To build this project run

```
make all;
```

## Running

To run the project on Cirrus specify the threads and then run

Frontend

```
export OMP_NUM_THREADS=$NO_OF_THREADS;
./loops2;
```

Backend

```
qsub loops2.pbs;
```

## Documentation

The header file affinity_scheduling.h and source file affinity_scheduling.c contain inline documentation for the used functions
