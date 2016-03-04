CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -fopenmp

OBJ_DIR = obj
OPENMP_DIR = OpenMP

omp: gtmp.c $(OBJ_DIR)/sense.o
	$(CC) -o gtmp $(OBJ_DIR)/sense.o $< $(CFLAGS) $(LDFLAGS)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/sense.o: $(OPENMP_DIR)/sense.c | $(OBJ_DIR)
	$(CC) -c -o $@ $< $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf $(OBJ_DIR)/
	rm -f *.o gtmp gtmpi
