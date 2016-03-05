#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <omp.h>

/*
    From the MCS Paper: A scalable, distributed tree-based barrier with only local spinning.

    type treenode = record
        parentsense : Boolean
	    parentpointer : ^Boolean
	    childpointers : array [0..1] of ^Boolean
	    havechild : array [0..3] of Boolean
	    childnotready : array [0..3] of Boolean
	    dummy : Boolean //pseudo-data

    shared nodes : array [0..P-1] of treenode
        // nodes[vpid] is allocated in shared memory
        // locally accessible to processor vpid
    processor private vpid : integer // a unique virtual processor index
    processor private sense : Boolean

    // on processor i, sense is initially true
    // in nodes[i]:
    //    havechild[j] = true if 4 * i + j + 1 < P; otherwise false
    //    parentpointer = &nodes[floor((i-1)/4].childnotready[(i-1) mod 4],
    //        or dummy if i = 0
    //    childpointers[0] = &nodes[2*i+1].parentsense, or &dummy if 2*i+1 >= P
    //    childpointers[1] = &nodes[2*i+2].parentsense, or &dummy if 2*i+2 >= P
    //    initially childnotready = havechild and parentsense = false
	
    procedure tree_barrier
        with nodes[vpid] do
	        repeat until childnotready = {false, false, false, false}
	    childnotready := havechild //prepare for next barrier
	    parentpointer^ := false //let parent know I'm ready
	    // if not root, wait until my parent signals wakeup
	    if vpid != 0
	        repeat until parentsense = sense
	    // signal children in wakeup tree
	    childpointers[0]^ := sense
	    childpointers[1]^ := sense
	    sense := not sense
*/

typedef struct {
    bool parentsense;
    bool *parentpointer;
    bool *childpointers[2];
    bool havechild[4];
    bool childnotready[4];
    bool dummy;
} treenode;

int num_threads;

treenode* nodes;

void gtmp_init();

void gtmp_barrier(int, bool);

void gtmp_finalize();

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: ./sense NUM_THREADS NUM_ITERS\n");
        return -1;
    }

    num_threads = atoi(argv[1]);
    int iters = atoi(argv[2]);
    omp_set_num_threads(num_threads);

    gtmp_init();
    #pragma omp parallel default(shared)
    {
        bool local_sense = true;
        int tid = omp_get_thread_num();
        int i;
        double start, end;
        start = omp_get_wtime();
        for (i = 0;i < iters;i++) {
            // printf("thread %d reaches the barrier!\n", omp_get_thread_num());
            // fflush(stdout);
            gtmp_barrier(tid, local_sense);
            local_sense = !local_sense;
            // printf("thread %d gets through the barrier!\n", omp_get_thread_num());
            // fflush(stdout);
        }
        end = omp_get_wtime();
        printf("time: %f\n", end - start);
    }

    gtmp_finalize();
    return 0;
}

void gtmp_init(){
    nodes = (treenode *) malloc(num_threads * sizeof(treenode));
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

void gtmp_barrier(int tid, bool sense){
    int i;
    for (i = 0;i < 4;i++) {
        while (nodes[tid].childnotready[i]) ;
        nodes[tid].childnotready[i] = nodes[tid].havechild[i];
    }
    *(nodes[tid].parentpointer) = false;
    if (tid > 0) {
        while (nodes[tid].parentsense != sense) ;
    }
    *(nodes[tid].childpointers[0]) = sense;
    *(nodes[tid].childpointers[1]) = sense;
}

void gtmp_finalize(){
    free(nodes);
}
