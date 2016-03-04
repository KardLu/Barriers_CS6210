CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -fopenmp

BIN_DIR = bin
OBJ_DIR = obj
OPENMP_DIR = OpenMP

sense: $(OPENMP_DIR)/sense.c $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/sense $< $(CFLAGS) $(LDFLAGS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(BIN_DIR)/
	rm -rf $(OBJ_DIR)/
	rm -f *.o gtmp gtmpi
