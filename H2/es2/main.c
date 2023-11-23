#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <omp.h>

#define dtype double
#define DBG_CHECK {printf("DBG_CHECK: file %s, line %d\n", __FILE__, __LINE__);}
#define BLOCK_SIZE 32

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
void matT(const struct matrix* in, struct matrix* out) {
   out->num_rows = in->num_cols;
   out->num_cols = in->num_rows;
   out->values = (dtype*)malloc(out->num_rows * out->num_cols * sizeof(dtype));

   for (int i = 0; i < in->num_rows; ++i) {
      for (int j = 0; j < in->num_cols; ++j) {
         out->values[j * out->num_rows + i] = in->values[i * in->num_cols + j];
      }
   }
}

//------------------------------------------------------
// Parallel
//------------------------------------------------------
void matTpar(const struct matrix* in, struct matrix* out) {
   out->num_rows = in->num_cols;
   out->num_cols = in->num_rows;
   out->values = (dtype*)malloc(out->num_rows * out->num_cols * sizeof(dtype));
   #pragma omp parallel for collapse(2) shared(in, out)
   for (int i = 0; i < in->num_rows; ++i) {
      for (int j = 0; j < in->num_cols; ++j) {
         out->values[j * out->num_rows + i] = in->values[i * in->num_cols + j];
      }
   }
}

//------------------------------------------------------
// Parallel with blocks
//------------------------------------------------------
void matBlockTpar(const struct matrix* in, struct matrix* out) {
   out->num_rows = in->num_cols;
   out->num_cols = in->num_rows;
   out->values = (dtype*)malloc(out->num_rows * out->num_cols * sizeof(dtype));
   #pragma omp parallel for collapse(2) shared(in, out)
   for (int bi = 0; bi < in->num_rows; bi += BLOCK_SIZE) {
      for (int bj = 0; bj < in->num_cols; bj += BLOCK_SIZE) {
         for (int i = bi; i < bi + BLOCK_SIZE && i < in->num_rows; ++i) {
            for (int j = bj; j < bj + BLOCK_SIZE && j < in->num_cols; ++j) {
               out->values[j * out->num_rows + i] = in->values[i * in->num_cols + j];
            }
         }
      }
   }
}

int main(int argc, char** argv){
   double start_time, end_time;

   struct matrix A = {300, 300};
   struct matrix A_T = {300, 300}; // init
   
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
   printf("%f\n", elapsed_time);

   start_time = omp_get_wtime();
   matTpar(&A, &A_T);
   end_time = omp_get_wtime();
   elapsed_time = end_time - start_time; // Elapsed time in seconds
   printf("%f\n", elapsed_time);

   start_time = omp_get_wtime();
   matBlockTpar(&A, &A_T);
   end_time = omp_get_wtime();
   elapsed_time = end_time - start_time; // Elapsed time in seconds
   printf("%f\n", elapsed_time);

   // Calculate memory bandwidth
   long long total_elements = A.num_rows * A.num_cols;
   double total_size_bytes = total_elements * sizeof(dtype);
   double bandwidth = total_size_bytes / (elapsed_time * 1e9); // GB/s
   double peak_bandwidth = 21.3; // Peak bandwidth GB/s
   double bandwidth_percentage = bandwidth / peak_bandwidth * 100;

   printf("%f\n", bandwidth);

   // PRINT_MATRIX(A);
   // PRINT_MATRIX(A_T);
   return 0;
}