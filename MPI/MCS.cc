/**********************************************************************
	MCS.cc
	
	This file contains implementation of MCS barrier using OpenMPI.
	The tree is a 4-ary tree.

	By Zenong Lu, March 4 2016
**********************************************************************/
#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>

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

	// ignore first param
	argc--;
	argv++;
	int numOfRound = 0;
	while (argc > 0 && argv[0][0] == '-') {
		if (argv[0][1] == 'r') {
			argc--;
			argv++;
			numOfRound = atoi(argv[0]);
		}
	}

	clock_t start = clock();
	for (int i = 0; i < numOfRound; ++i) {
		MCS_Barrier();
	}
	clock_t end = clock();

	std::cout << (double) (end - start) / CLOCKS_PER_SEC << std::endl;

	MPI_Finalize();
	return 0;
}