#IFNDEF __GTMPI_H__
#DEFINE __GTMPI_H__

/*
 *  Header for MPI-based barrier
 */ 

void gtmpi_init(int num_threads);

void gtmpi_barrier();

void gtmpi_finalize();

#ENDIF