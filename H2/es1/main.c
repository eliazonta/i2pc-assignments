#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <omp.h>


#define dtype double
#define DBG_CHECK {printf("DBG_CHECK: file %s, line %d\n", __FILE__, __LINE__);}
#define BLOCK_SIZE 32
#define DATA_OUT(wt1, wt2, n) {printf("%d %12.4g\n", n, (wt2 - wt1));} // s

#define PRINT_MATRIX(M) { \
   printf("Matrix %s:\n", #M); \
      for (int i = 0; i < M.num_rows; ++i) { \
         for (int j = 0; j < M.num_cols; ++j) { \
            printf("%lf ", M.values[i * M.num_cols + j]); \
         } \
         printf("\n"); \
      } \
}

#define CANNOT_MULTIPLY(ncA, nrB){\
   if(ncA != nrB) {\
      printf("Error: A.num_cols != B.num_rows - cannot multiply\n");\
      exit(1);\
   }\
}

struct matrix{
   int num_rows, num_cols; //m, n
   dtype* values;
};
double wt1, wt2;


//------------------------------------------------------
// Serial
//------------------------------------------------------
void matMul(const struct matrix* A, const struct matrix* B, struct matrix* C) {
   CANNOT_MULTIPLY(A->num_cols, B->num_rows);
   C->num_rows = A->num_rows;
   C->num_cols = B->num_cols;
   C->values = (dtype*)malloc(C->num_rows * C->num_cols * sizeof(dtype));
   wt1 = omp_get_wtime();
   for (int i = 0; i < A->num_rows; ++i) {
      for (int j = 0; j < B->num_cols; ++j) {
         C->values[i * (C->num_cols) + j] = 0.0;
         for (int l = 0; l < C->num_cols; ++l) {
            C->values[i * (C->num_cols) + j] += A->values[i * (A->num_cols) + l] * B->values[l * (B->num_cols) + j];
         }
      }
   }
   wt2 = omp_get_wtime();
   // printf("matMul [Serial] - Elapsed : %12.4g sec\n", wt2 - wt1);
}

//------------------------------------------------------
// Parallel
//------------------------------------------------------
void matMulPar(const struct matrix* A, const struct matrix* B, struct matrix* C) {
   CANNOT_MULTIPLY(A->num_cols, B->num_rows);
   C->num_rows = A->num_rows;
   C->num_cols = B->num_cols;
   C->values = (dtype*)malloc(C->num_rows * C->num_cols * sizeof(dtype));
   wt1 = omp_get_wtime();
   #pragma omp parallel for collapse(2)
   for (int i = 0; i < A->num_rows; ++i) {
      for (int j = 0; j < B->num_cols; ++j) {
         C->values[i * (C->num_cols) + j] = 0.0;
         for (int l = 0; l < C->num_cols; ++l) {
            C->values[i * (C->num_cols) + j] += A->values[i * (A->num_cols) + l] * B->values[l * (B->num_cols) + j];
         }
      }
   }
   wt2 = omp_get_wtime();
   // printf("%12.4g\n", wt2 - wt1);
}

int main(int argc, char** argv) {
   srand(time(NULL));
   // toy matrices
   int nrA = 100, ncA = 100, nrB = 100, ncB = 100;
   struct matrix A = {nrA, ncA}, B = {nrB, ncB}, C = {nrA, ncB};;
   A.values = (dtype*)malloc(A.num_rows * A.num_cols * sizeof(dtype));
   B.values = (dtype*)malloc(B.num_rows * B.num_cols * sizeof(dtype));

   CANNOT_MULTIPLY(ncA, nrB);

   C.values = (dtype*)malloc(C.num_rows * C.num_cols * sizeof(dtype));

   for (int i = 0; i < A.num_rows * A.num_cols; ++i)
      A.values[i] = rand()%100;
   for (int i = 0; i < B.num_rows * B.num_cols; ++i)
      B.values[i] = 1.0;


   double start, end;  
   double total_flops = 2.0 * A.num_rows * A.num_cols * B.num_cols; 

   start = omp_get_wtime();
   // matMul(&A, &B, &C); 
   matMulPar(&A, &B, &C); 
   end = omp_get_wtime();

   double execution_time = end - start;  
   double flops = total_flops / (execution_time * 1e-9); // GFLOPS 
   // printf("%lf,%d\n", flops, omp_get_num_threads()); 
   DATA_OUT(wt1, wt2, omp_get_max_threads());

   // PRINT_MATRIX(A);
   // PRINT_MATRIX(B);
   // PRINT_MATRIX(C);
   free(A.values);
   free(B.values);
   free(C.values);

   return 0;
}
