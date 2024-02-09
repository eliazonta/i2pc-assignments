CC := gcc
CXX := g++
MPICC := mpicxx

CFLAGS := -std=c99 -O3
CXXFLAGS := -std=c++11 -O3
OMPFLAGS := -fopenmp
MAIN := main.c
TARGET := es2.out

all: sequential parallel blocks

sequential: sequential.c
	@echo "Compiling..."
	$(CC) $(CFLAGS) -o sequential sequential.c

parallel: parallel.cpp
	@echo "Compiling..."
	$(MPICC) $(CXXFLAGS) -o parallel parallel.cpp

blocks: parallel-blocks.cpp
	@echo "Compiling..."
	$(MPICC) $(CXXFLAGS) -o parallel-blocks parallel-blocks.cpp
clean:
	rm -f *.o *.a *.so *.out *.gch *.o *.e sequential parallel parallel-blocks
