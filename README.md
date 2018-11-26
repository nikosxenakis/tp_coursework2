
# Threaded Programming Coursework Part 2

## Frontend

### Building

To build this project run

```
make loops2;
```

### Running

To run the project on the frontend of Cirrus run

```
export OMP_NUM_THREADS=$NO_OF_THREADS;
./loops2;
```

## Backend
	
### Building & Running

To build and run the project on Cirrus backend run the script

```
./run.sh
```

This script will create the appropriate folders and targets then it will submit the project

### Graphs
To parse the data and generate the report graphs run
```
python data_analyzer.py
```

## Documentation

The header file affinity_scheduling.h and source file affinity_scheduling.c contain inline documentation for the used functions
