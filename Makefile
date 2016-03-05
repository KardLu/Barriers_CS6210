CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -fopenmp

BIN_DIR = bin
OBJ_DIR = obj
OPENMP_DIR = OpenMP

all: omp_sense omp_tree

omp_sense: $(OPENMP_DIR)/sense.c $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/sense $< $(CFLAGS) $(LDFLAGS)

omp_tree: $(OPENMP_DIR)/tree.c $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/tree $< $(CFLAGS) $(LDFLAGS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(BIN_DIR)/
	rm -rf $(OBJ_DIR)/
	rm -f *.o gtmp gtmpi
