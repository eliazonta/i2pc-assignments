#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <mpi.h>
#include <sys/time.h>
#include <chrono>

#define DEBUG false


void matBlockTpar(double *matrix, double *transposed_matrix, int n, int block_size, MPI_Comm comm) 
{
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    // Scatter the matrix blocks across processes
    double *local_block = new double[block_size * block_size];
    MPI_Scatter(matrix, block_size * n, MPI_DOUBLE, local_block, block_size * block_size, MPI_DOUBLE, 0, comm);

    // Transpose local blocks
    for (int i = 0; i < block_size; ++i) 
    {
        for (int j = 0; j < block_size; ++j) 
        {
            transposed_matrix[i * size * block_size + rank * block_size + j] = local_block[j * block_size + i];
        }
    }

    // Gather the transposed blocks to construct the final transposed matrix
    MPI_Gather(transposed_matrix + rank * block_size * block_size, block_size * block_size, MPI_DOUBLE,
           transposed_matrix, block_size * block_size, MPI_DOUBLE, 0, comm);

    delete[] local_block;
}

int main(int argc, char *argv[]) 
{
    auto start_time = std::chrono::high_resolution_clock::now();
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int n = 1000;  // square matrix 
    int block_size = n / size;

    double *matrix = nullptr;
    double *transposed_matrix = nullptr;

    if (rank == 0) 
    {
        // Initialize and populate the matrix on process 0
        matrix = new double[n * n];
        for (int i = 0; i < n * n; ++i) 
        {
            matrix[i] = i;
        }

        // Allocate memory for the transposed matrix
        transposed_matrix = new double[n * n];
    }

    // Scatter the matrix to all processes
    // cast blocksize to void* to avoid warning outside the MPI call 
    MPI_Bcast(&block_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(matrix, n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // parallel matrix transpose
    matBlockTpar(matrix, transposed_matrix, n, block_size, MPI_COMM_WORLD);

    if (rank == 0) 
    {
        #if DEBUG
        // Print the original and transposed matrices
        std::cout << "Original Matrix:" << std::endl;
        for (int i = 0; i < n; ++i) 
        {
            for (int j = 0; j < n; ++j) 
            {
                std::cout << matrix[i * n + j] << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "\nTransposed Matrix:" << std::endl;
        for (int i = 0; i < n; ++i) 
        {
            for (int j = 0; j < n; ++j) 
            {
                std::cout << transposed_matrix[i * n + j] << " ";
            }
            std::cout << std::endl;
        }
        #endif
        delete[] matrix;
        delete[] transposed_matrix;
    }

    MPI_Finalize();
    auto end_time = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    double elapsed_time = duration.count() / 1000.0;  // s

    double data_size = (n * n * sizeof(double) * 2) / (1024.0 * 1024.0);
    double bandwidth = data_size / elapsed_time; // 
    printf("%f s\t%f GB/s\n", elapsed_time, bandwidth);
    printf("%f\n", bandwidth);

    return 0;
}

