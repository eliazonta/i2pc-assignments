CC := gcc
CFLAGS := -std=c99 -O3 -pg
OMPFLAGS := -fopenmp
MAIN := main.c
TARGET := es2.out

all: $(TARGET)

$(TARGET) : $(MAIN)
	@echo "Compiling..."
	$(CC) $(CFLAGS) $(OMPFLAGS) -o $(TARGET) $(MAIN)
clean:
	rm -f *.o *.a *.so *.out *.gch *.o *.e 
