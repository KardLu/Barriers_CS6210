#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <cstdlib>
#include <cstdbool>
#include <ctime>

// number of processes
int size = -1;

// process rank
int rank = -1;

// number of threads
int count;

// global sense variable
bool sense;

bool havechild[4];
bool childnotready[4];
bool awakechild[2];

void omp_barrier(bool *);

void mpi_init();

void mpi_barrier();

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv); 
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    mpi_init();

    if (argc < 3) {
        fprintf(stderr, "Usage: ./combine NUM_THREADS NUM_ITERS\n");
        return -1;
    }

    count = atoi(argv[1]);
    sense = true;
    omp_set_num_threads(count);
    int iters = atoi(argv[2]);

    clock_t start = clock();
    #pragma omp parallel default(shared)
    {
        bool local_sense = true;
        int tid = omp_get_thread_num();
        int i;
        for (i = 0;i < iters;i++) {
            printf("thread %d from process %d reaches the barrier!\n", tid, rank);
            fflush(stdout);
            omp_barrier(&local_sense);
        }
    }
    clock_t end = clock();
    printf("time: %lf\n", (double) (end - start) / CLOCKS_PER_SEC);

    MPI_Finalize();
    return 0;
}

void omp_barrier(bool *local_sense) {
    *local_sense = !(*local_sense);

    if (__sync_fetch_and_sub(&count, 1) == 1) {
        count = omp_get_num_threads();
        mpi_barrier();
        sense = *local_sense;
    }
    else {
        while (sense != (*local_sense));
    }
}

void mpi_init() {
    for (int i = 0; i < 4; i++) {
        if (4 * rank + i + 1 < size) {
            havechild[i] = true;
            childnotready[i] = true;
        }
        else {
            havechild[i] = false;
            childnotready[i] = false;
        }
    }
    for (int i = 0; i < 2; i++) 
        awakechild[i] = (2 * rank + 1 + i < size);
}

void mpi_barrier() 
{
    // wait for all children
    int msg;
    MPI_Status status;
    while (childnotready[0] || 
           childnotready[1] || 
           childnotready[2] || 
           childnotready[3]) {
        MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        childnotready[status.MPI_SOURCE - 4 * rank - 1] = false; // may need error check
    }

    // prepare for next round
    for (int i = 0; i < 4; i++)
        childnotready[i] = havechild[i];

    msg = 1;
    if (rank != 0) {
        // tell parent I'm ready
        MPI_Send(&msg, 1, MPI_INT, (rank - 1) / 4, 0, MPI_COMM_WORLD);

        // wait for parent
        MPI_Recv(&msg, 1, MPI_INT, (rank - 1) / 2, 1, MPI_COMM_WORLD, NULL);
    }

    // awake children
    if (awakechild[0])
        MPI_Send(&msg, 1, MPI_INT, 2 * rank + 1, 1, MPI_COMM_WORLD);
    if (awakechild[1])
        MPI_Send(&msg, 1, MPI_INT, 2 * rank + 2, 1, MPI_COMM_WORLD);
}
