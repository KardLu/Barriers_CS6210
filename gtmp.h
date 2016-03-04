#ifndef __GTMP_H__
#define __GTMP_H__

#include <stdbool.h>

/*
 *  Header for OpenMP-based barrier
 */ 

int count;

bool sense;

bool local_sense;

void gtmp_init(int num_threads);

void gtmp_barrier();

void gtmp_finalize();

#endif