#include <omp.h>
#include <stdio.h>
#include "gtmp.h"

#define NUM_THREADS 5

int main(int argc, char **argv) {
	omp_set_num_threads(NUM_THREADS);
	gtmp_init(NUM_THREADS);

	#pragma omp parallel shared(count,sense) firstprivate(local_sense)
	{
		printf("thread %d reaches the barrier!\n", omp_get_thread_num());
		fflush(stdout);
		gtmp_barrier();
		printf("thread %d gets through the barrier!\n", omp_get_thread_num());
		fflush(stdout);
	}
	gtmp_finalize();
	return 0;
}
