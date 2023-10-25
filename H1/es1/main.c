#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
// #include <arm_neon.h> // for arm
// #include <immintrin.h> // for x86  
#include <omp.h>

#define dtype float
#define DBG_CHECK {printf("DBG_CHECK: file %s, line %d\n", __FILE__, __LINE__);}
#define DATA_OUT(wt1, wt2, n) {printf("%d %12.4g\n", n, (wt2 - wt1) * 1000);} // ms // time in ms for the plots

#define PRINT_ARRAY(A, N) \
    for (int i = 0; i < N; ++i) { \
        printf("%.3f ", A[i]); \
    } \
    printf("\n");

double wt1, wt2;

void routine1(dtype* a, dtype* b, dtype* c, int n){
    wt1 = omp_get_wtime();
    int i;
    for(i = 0; i < n; ++i){
        c[i] = a[i] + b[i];
    }
    wt2 = omp_get_wtime();
    printf("Routine 1 [serial] - Elapsed : %12.4g sec\n", wt2 - wt1);
    // DATA_OUT(wt1, wt2, n);
}

//-----------------------------------
// routine with vectorization 
//-----------------------------------
void routine2(dtype* a, dtype* b, dtype* c, int n){
    wt1 = omp_get_wtime();
    int i;
    #pragma omp simd
    for (i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
    wt2 = omp_get_wtime();
    printf("Routine 2 [vectorization] - Elapsed : %12.4g sec\n", wt2 - wt1);
    // DATA_OUT(wt1, wt2, n);
}

//-----------------------------------
// routine with vectorization for arm [has to be compiled for the specific architecture w/ the lib]
//-----------------------------------
// void routine2arm(dtype* a, dtype* b, dtype* c, int n){
//     wt1 = omp_get_wtime();
//     int step = 2;
//     for (int i = 0; i < n; i += 8) {
//         // Load 8 floats from arrays a and b into SIMD registers
//         float32x4_t va = vld1q_f32(&a[i]);
//         float32x4_t vb = vld1q_f32(&b[i]);
//         float32x4_t result = vaddq_f32(va, vb);
//         vst1q_f32(&c[i], result);
//     }
//     wt2 = omp_get_wtime();
//     printf("Routine 2 [ARM] - Elapsed : %12.4g", wt2 - wt1);
//     DATA_OUT(wt1, wt2, n);
// }


//-----------------------------------
// routine with vectorization for x86 [has to be compiled for the specific architecture w/ the lib]
//-----------------------------------
// void routine2intel(dtype* a, dtype* b, dtype* c, int n){
//     wt1 = omp_get_wtime();
//     int step = 2;
//     for (int i = 0; i < n; i += 8) {
//         // Load 8 floats from arrays a and b into SIMD registers
//         __m256 va = _mm256_loadu_ps(&a[i]);
//         __m256 vb = _mm256_loadu_ps(&b[i]);
//         __m256 result = _mm256_add_ps(va, vb);
//         _mm256_storeu_ps(&c[i], result);
//     }
//     wt2 = omp_get_wtime();
//     printf("Routine 2 [Intel] - Elapsed : %12.4g", wt2 - wt1);
//     DATA_OUT(wt1, wt2, n);
// }


//-----------------------------------
// prefetching routine 
//-----------------------------------
void routine3(dtype* a, dtype* b, dtype* c, int n){
    wt1 = omp_get_wtime();
    int i;
    for(i = 0; i < n; ++i){
        __builtin_prefetch(&a[i]);
        __builtin_prefetch(&b[i]);
        c[i] = a[i] + b[i];
    }
    wt2 = omp_get_wtime();
    printf("Routine 3 [prefetching] - Elapsed : %12.4g sec\n", wt2 - wt1);
    // DATA_OUT(wt1, wt2, n);
}


int main(int argc, char** argv){
    int n;
    if(argc != 2){
        printf("Usage: %s <input dim power>\n", argv[0]);
        printf("<input dim> = 4 means dim = 2^4 = 16\n");
        printf("Using default input dim = 16\n");
        n = 1 << 4;
    }else{
        n = 1 << atoi(argv[1]);
        printf("Using input dim: %d\n", n);
    }
    srand(time(NULL));

    if(n < 1<<4 || n > 1<<22){
        printf("Invalid input range.\n");
        return 1;
    }

    dtype* A = (dtype*)malloc(n * sizeof(dtype));
    dtype* B = (dtype*)malloc(n * sizeof(dtype));
    dtype* C = (dtype*)malloc(n * sizeof(dtype));
    memset(A, 0, n * sizeof(dtype));
    memset(B, 0, n * sizeof(dtype));
    memset(C, 0, n * sizeof(dtype));
    
    int i;
    for (i = 0; i < n; ++i) {
        A[i] = rand() % 100; 
        B[i] = rand() % 100;
    }
    if (A == NULL || B == NULL || C == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }
    routine1(A, B, C, n);
    routine2(A, B, C, n);
    // routine2intel(A, B, C, n);
    // routine2arm(A, B, C, n);
    routine3(A, B, C, n);
    // PRINT_ARRAY(A, n);
    // PRINT_ARRAY(B, n);
    // PRINT_ARRAY(C, n);
    return 0;
}