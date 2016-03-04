#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int count;
bool sense;
void gtmp_barrier();

int main(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "Usage: ./sense NUM_THREADS NUM_ITERS\n");
		return -1;
	}

	count = atoi(argv[1]);
	int iters = atoi(argv[2]);
	sense = true;
	omp_set_num_threads(count);

	#pragma omp parallel shared(count,sense)
	{
		bool local_sense = true;
		int i;
		for (i = 0;i < iters;i++) {
			printf("thread %d reaches the barrier!\n", omp_get_thread_num());
			fflush(stdout);
			gtmp_barrier(&local_sense);
			printf("thread %d gets through the barrier!\n", omp_get_thread_num());
			fflush(stdout);
		}
	}
	return 0;
}

void gtmp_barrier(bool *local_sense){
	*local_sense = !(*local_sense);

	#pragma omp atomic
	count -= 1;

	if (count == 0) {
		count = omp_get_num_threads();
		sense = *local_sense;
	}
	else {
		while (sense != (*local_sense)) {}
	}
}
