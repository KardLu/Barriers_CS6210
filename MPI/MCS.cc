/**********************************************************************
	MCS.cc
	
	This file contains implementation of MCS barrier using OpenMPI.
	The tree is a 4-ary tree.

	By Zenong Lu, March 4 2016
**********************************************************************/

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

#include <mpi.h>
#include <iostream>

int size = -1;
int rank = -1;
bool havechild[4];
bool childnotready[4];
bool awakechild[2];

void MCS_Barrier() 
{
	// init
	for (int i = 0; i < 4; i++) {
		if ( 4 * rank + i + 1 < size) 
			havechild[i] = true;
		childnotready[i] = havechild[i];
	}
	for (int i = 0; i < 2; i++) 
		awakechild[i] = (2 * rank + 1 + i < size);	

	// wait for all children
	while (childnotready[0] || 
		   childnotready[1] || 
		   childnotready[2] || 
		   childnotready[3]) {
		int msg;
		MPI_Status status;
		MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		childnotready[status.MPI_SOURCE - 4 * rank - 1] = false; // may need error check
	}

	// prepare for next round
	for (int i = 0; i < 4; i++)
		childnotready[i] = havechild[i];

	if (rank != 0) {
		// tell parent I'm ready
		int msg = 1;
		MPI_Send(&msg, 1, MPI_INT, (rank - 1) / 4, 0, MPI_COMM_WORLD);

		// wait for parent
		MPI_Recv(&msg, 1, MPI_INT, (rank - 1) / 2, 1, MPI_COMM_WORLD, NULL);
	}

	// awake children
	int msg = 1;
	if (awakechild[0])
		MPI_Send(&msg, 1, MPI_INT, 2 * rank + 1, 1, MPI_COMM_WORLD);
	if (awakechild[1])
		MPI_Send(&msg, 1, MPI_INT, 2 * rank + 2, 1, MPI_COMM_WORLD);
}

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);	
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);


	MCS_Barrier();
	std::cout<< "Size: " << size <<", Rank: " << rank << std::endl;

	MPI_Finalize();
	return 0;
}