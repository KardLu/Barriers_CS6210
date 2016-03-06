#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// global counter variable
int count;

// global sense variable
bool sense;

void omp_barrier();

int main(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "Usage: ./sense NUM_THREADS NUM_ITERS\n");
		return -1;
	}

	count = atoi(argv[1]);
	int iters = atoi(argv[2]);
	sense = true;
	omp_set_num_threads(count);

	clock_t start, end;
	start = clock();
	#pragma omp parallel shared(count,sense)
	{
		bool local_sense = true;
		int i;
		for (i = 0;i < iters;i++) {
			// printf("thread %d reaches the barrier!\n", omp_get_thread_num());
			// fflush(stdout);
			omp_barrier(&local_sense);
		}
	}
	end = clock();
	printf("time: %lf\n", (double) (end - start) / CLOCKS_PER_SEC);
	return 0;
}

void omp_barrier(bool *local_sense) {
	*local_sense = !(*local_sense);

	if (__sync_fetch_and_sub(&count, 1) == 1) {
		count = omp_get_num_threads();
		sense = *local_sense;
	}
	else {
		while (sense != (*local_sense));
	}
}
