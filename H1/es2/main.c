#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>

#define dtype int
#define DBG_CHECK {printf("DBG_CHECK: file %s, line %d\n", __FILE__, __LINE__);}

// block size, time in ms and bandwidth for the plots
// bandwidth in GB/s and in GFLOPS
#define DATA_OUT(wt1, wt2, n, b) {\
    double totflops = (double)n * n;\
    double bw = (totflops * sizeof(dtype) / (1e9 * (wt2 - wt1)));\
    printf("%d %d %12.6g %12.6g \n", n, b, (wt2 - wt1) * 1000, bw);\
} 

#define PRINT_MATRIX(A, N) \
    for (int i = 0; i < N; ++i) { \
        for (int j = 0; j < N; ++j) { \
            printf("%d ", A[i][j]); \
        } \
        printf("\n"); \
    } \
    printf("\n");

double wt1, wt2;

//-----------------------------------
// serial routine  
//-----------------------------------
void routine1(dtype** A, dtype** O, int n, int b) {
    wt1 = omp_get_wtime();
    int i, j, p, q;
    for (i = 0; i < n; i += b) {
        for (j = 0; j < n; j += b) {
            for (p = 0; p < b; ++p) {
                for (q = 0; q < b; ++q) {
                    O[i + p][j + q] = A[i + b - 1 - p][j + b - 1 - q];
                }
            }
        }
    }
    wt2 = omp_get_wtime();
    // printf("Routine 1 [serial] - Elapsed : %12.4g sec\n", wt2 - wt1);
    DATA_OUT(wt1, wt2, n, b);
}

//-----------------------------------
// Prefetching routine  (to me seeems more viable for the task)
//-----------------------------------
void routine2(dtype** A, dtype** O, int n, int b) {
    wt1 = omp_get_wtime();
    int i, j, p, q;
    for (i = 0; i < n; i += b) {
        for (j = 0; j < n; j += b) {
            for (p = 0; p < b; ++p) {
                for (q = 0; q < b; ++q) {
                    __builtin_prefetch(&A[i + b - 1 - p][j + b - 1 - q]);
                    __builtin_prefetch(&O[i + p][j + q]);
                    O[i + p][j + q] = A[i + b - 1 - p][j + b - 1 - q];
                }
            }
        }
    }
    wt2 = omp_get_wtime();
    // printf("Routine 2 [prefetching] - Elapsed : %12.4g sec\n", wt2 - wt1);
    DATA_OUT(wt1, wt2, n, b);
}

int main(int argc, char** argv){
    int n = 1<< 12;  // Size
    int b = 1<<2;  // Block dim 
    srand(time(NULL));

    int** M = (dtype**)malloc(n * sizeof(dtype*));
    int** O = (dtype**)malloc(n * sizeof(dtype*));
    for (int i = 0; i < n; ++i) {
        M[i] = (dtype*)malloc(n * sizeof(dtype));
        O[i] = (dtype*)malloc(n * sizeof(dtype));
    }

    // matrix init
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            M[i][j] = rand() % 100; 
        }
    }
    for(int i = 2; i < 9; ++i){
        b = 1 << i;
        routine1(M, O, n, b);
        // routine2(M, O, n, b);
    }
    
    

    // printf("Input Matrix M:\n");
    // PRINT_MATRIX(M, n);

    // printf("Output Matrix O:\n");
    // PRINT_MATRIX(O, n);

    // Free mem
    for (int i = 0; i < n; ++i) {
        free(M[i]);
        free(O[i]);
    }
    free(M);
    free(O);

    return 0;
}