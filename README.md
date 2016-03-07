# CS6210 Project 2: Barrier Synchronization
Some barriers implementations for project 2 on CS6210 using OpenMP and MPI.

## how to run
Make sure that `gcc`, `mpicc`, and `mpic++` are in `PATH`. Just type `make` and all the executables will be in the `bin` directory.

For OpenMP-based barriers, the test can be run via `bin/sense NUM_THREADS NUM_ITER` and `bin/tree NUM_THREADS NUM_ITER`.

For MPI-based barriers, the test can be run via `mpirun -np NUM_PROC bin/tournament -r NUM_ITER` and `mpirun -np NUM_PROC bin/mcs -r NUM_ITER`.

For combined barrier, the test can be run as `mpirun -np NUM_PROC bin/combine NUM_THREADS NUM_ITER`.