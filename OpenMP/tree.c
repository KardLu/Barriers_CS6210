#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <omp.h>
#include <time.h>

typedef struct {
    bool parentsense;
    bool *parentpointer;
    bool *childpointers[2];
    bool havechild[4];
    bool childnotready[4];
    bool dummy;
    // padding for the node; only works for 64 bytes cache line
    int padding[4];
} treenode;

int num_threads;

// shared array
treenode* nodes;

void omp_init();

void omp_barrier(int, bool);

void omp_finalize();

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: ./sense NUM_THREADS NUM_ITERS\n");
        return -1;
    }

    num_threads = atoi(argv[1]);
    int iters = atoi(argv[2]);
    omp_set_num_threads(num_threads);
    omp_init();

    clock_t start, end;
    start = clock();
    #pragma omp parallel default(shared)
    {
        bool local_sense = true;
        int tid = omp_get_thread_num();
        int i;
        for (i = 0;i < iters;i++) {
            // printf("thread %d reaches the barrier!\n", omp_get_thread_num());
            // fflush(stdout);
            omp_barrier(tid, local_sense);
            local_sense = !local_sense;
        }
    }
    end = clock();
    printf("time: %lf\n", (double) (end - start) / CLOCKS_PER_SEC);

    omp_finalize();
    return 0;
}

void omp_init(){
    posix_memalign((void **) &nodes, LEVEL1_DCACHE_LINESIZE, num_threads * LEVEL1_DCACHE_LINESIZE);
    // nodes = (treenode *) malloc(num_threads * sizeof(treenode));
    int i, j;
    for (i = 0;i < num_threads;i++) {
        for (j = 0;j < 4;j++) {
            if ((4 * i + j + 1) < num_threads) {
                nodes[i].havechild[j] = true;
                nodes[i].childnotready[j] = true;
            }
            else {
                nodes[i].havechild[j] = false;
                nodes[i].childnotready[j] = false;
            }
        }
        if (i == 0)
            nodes[i].parentpointer = &(nodes[i].dummy);
        else
            nodes[i].parentpointer = &(nodes[(i-1)/4].childnotready[(i-1)%4]);
        if ((2 * i + 1) < num_threads)
            nodes[i].childpointers[0] = &(nodes[2*i+1].parentsense);
        else
            nodes[i].childpointers[0] = &(nodes[i].dummy);
        if ((2 * i + 2) < num_threads)
            nodes[i].childpointers[1] = &(nodes[2*i+2].parentsense);
        else
            nodes[i].childpointers[1] = &(nodes[i].dummy);
        nodes[i].parentsense = false;
    }
}

void omp_barrier(int tid, bool sense){
    int i;
    for (i = 0;i < 4;i++) {
        while (nodes[tid].childnotready[i]);
        nodes[tid].childnotready[i] = nodes[tid].havechild[i];
    }
    *(nodes[tid].parentpointer) = false;
    if (tid > 0) {
        while (nodes[tid].parentsense != sense) ;
    }
    *(nodes[tid].childpointers[0]) = sense;
    *(nodes[tid].childpointers[1]) = sense;
}

void omp_finalize(){
    free(nodes);
}
