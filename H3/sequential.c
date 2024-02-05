#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <omp.h>

#define dtype double
#define SIZE 300

#define PRINT_MATRIX(M) { \
   printf("Matrix %s:\n", #M); \
    for (int i = 0; i < M.num_rows; ++i) { \
        for (int j = 0; j < M.num_cols; ++j) { \
            printf("%lf ", M.values[i * M.num_cols + j]); \
        } \
        printf("\n"); \
    } \
}

struct matrix{
   int num_rows, num_cols; //m, n
   dtype* values;
};
double wt1, wt2;

int main()
{
    double start_time, end_time;
    struct matrix A = {SIZE, SIZE};
    struct matrix A_T = {SIZE, SIZE}; // init

    A.values = (dtype*)malloc(A.num_rows * A.num_cols * sizeof(dtype));
    A_T.values = (dtype*)malloc(A_T.num_rows * A_T.num_cols * sizeof(dtype));


    for (int i = 0; i < A.num_rows * A.num_cols; ++i)
        A.values[i] = rand()%100;
    for (int i = 0; i < A_T.num_rows * A_T.num_cols; ++i)
        A_T.values[i] = 0.0;


    start_time = omp_get_wtime();
    matT(&A, &A_T);
    end_time = omp_get_wtime();

    double elapsed_time = end_time - start_time; // Elapsed time in seconds
    double data_size = (SIZE * SIZE * sizeof(dtype) * 2) / (1024.0 * 1024.0);
    double bandwidth = data_size / elapsed_time; // 
    printf("%f s\t%f GB/s\n", elapsed_time, bandwidth);
    printf("%f\n", bandwidth);

    // PRINT_MATRIX(A);
    // PRINT_MATRIX(A_T);
    return 0;
}