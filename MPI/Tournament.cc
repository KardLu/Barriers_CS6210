/**********************************************************************
	Tournament.cc
	
	This file contains implementation of Tournament barrier using OpenMPI.

	By Zenong Lu, March 4 2016
**********************************************************************/
#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>

int rank = -1;
int size = -1;

void Tournament_Barrier()
{
	int round = 1;
	bool lose = false;

	while (!lose) {
		if (round >= size) 
			break;
		
		int rival = rank ^ round;
		round = round << 1;

		if (rival >= size) 
			continue;

		// small rank win, winner wait for loser, loser send msg to winner.
		if (rank < rival) {
			int msg = 1;
			MPI_Recv(&msg, 1, MPI_INT, rival, 0, MPI_COMM_WORLD, NULL);
		}else {
			int msg = 0;
			MPI_Send(&msg, 1, MPI_INT, rival, 0, MPI_COMM_WORLD);
			lose = true;
		}
	}

	if (rank == 0) {
		// Champion awake all nodes.
		while (round != 1) {
			round = round >> 1;
			int rival = rank ^ round;
			if (rival >= size) 
				continue;
			int msg = 1;
			MPI_Send(&msg, 1, MPI_INT, rival, 1, MPI_COMM_WORLD);
		}
	}else {
		// losers spin.
		bool awaked = false;
		while(round != 1) {
			round = round >> 1;
			int rival = rank ^ round;
			if (rival >= size) 
				continue;
			if (awaked) {
				int msg = 1;
				MPI_Send(&msg, 1, MPI_INT, rival, 1, MPI_COMM_WORLD);
			} else {
				int msg = 0;
				MPI_Recv(&msg, 1, MPI_INT, rival, 1, MPI_COMM_WORLD, NULL);	
				awaked = true;
			}
		}
	}
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
		Tournament_Barrier();
	}
	clock_t end = clock();

	double total = (double) (end - start) / CLOCKS_PER_SEC;
	std::cout << total / numOfRound << std::endl;

	MPI_Finalize();
	return 0;
}
