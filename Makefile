CC = gcc
CXX = mpic++
MPICC = mpicc
CFLAGS = -g -Wall
CXXFLAGS = -std=c++11
MPFLAGS = -fopenmp -DLEVEL1_DCACHE_LINESIZE=`getconf LEVEL1_DCACHE_LINESIZE`

BIN_DIR = bin
OBJ_DIR = obj
OPENMP_DIR = OpenMP
MPI_DIR = MPI

all: omp_sense omp_tree mpi_tournament mpi_mcs combine

omp_sense: $(OPENMP_DIR)/sense.c $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/sense $< $(CFLAGS) $(MPFLAGS)

omp_tree: $(OPENMP_DIR)/tree.c $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/tree $< $(CFLAGS) $(MPFLAGS)

mpi_tournament: $(MPI_DIR)/Tournament.cc $(BIN_DIR)
	$(CXX) -o $(BIN_DIR)/tournament $< $(CFLAGS)

mpi_mcs: $(MPI_DIR)/MCS.cc $(BIN_DIR)
	$(CXX) -o $(BIN_DIR)/mcs $< $(CFLAGS)

combine: combine.c $(BIN_DIR)
	$(MPICC) -o $(BIN_DIR)/combine $< $(CFLAGS) $(MPFLAGS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(BIN_DIR)/
	rm -rf $(OBJ_DIR)/
	rm -f *.o gtmp gtmpi
