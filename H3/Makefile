CC := gcc
CFLAGS := -std=c99 -O3
OMPFLAGS := -fopenmp
MAIN := main.c
TARGET := es2.out

all: $(TARGET)

sequential: sequential.c
	@echo "Compiling..."
	$(CC) $(CFLAGS) -o sequential sequential.c

parallel: parallel.cpp
	@echo "Compiling..."
	$(CC) $(CFLAGS) $(OMPFLAGS) -o parallel parallel.cpp


# WIP
$(TARGET) : $(MAIN)
	@echo "Compiling..."
	$(CC) $(CFLAGS) $(OMPFLAGS) -o $(TARGET) $(MAIN)

clean:
	rm -f *.o *.a *.so *.out *.gch *.o *.e
