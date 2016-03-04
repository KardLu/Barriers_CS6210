#ifndef __GTMPI_H__
#define __GTMPI_H__

/*
 *  Header for MPI-based barrier
 */ 

void gtmpi_init(int num_threads);

void gtmpi_barrier();

void gtmpi_finalize();

#endif