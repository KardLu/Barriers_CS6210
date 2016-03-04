#IFNDEF __GTMP_H__
#DEFINE __GTMP_H__

/*
 *  Header for OpenMP-based barrier
 */ 

void gtmp_init(int num_threads);

void gtmp_barrier();

void gtmp_finalize();

#ENDIF