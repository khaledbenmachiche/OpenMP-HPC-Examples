#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "../include/omp_utils.h"

#define SIZE 1000
#define ITERATIONS 3

void matrix_multiply_seq(double **A, double **B, double **C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0.0;
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void matrix_multiply_parallel(double **A, double **B, double **C, int n) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0.0;
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {
    double **A, **B, **C;
    double start, end;
    double seq_time = 0.0, parallel_time = 0.0;
    
    // Allocate memory
    A = (double**)malloc(SIZE * sizeof(double*));
    B = (double**)malloc(SIZE * sizeof(double*));
    C = (double**)malloc(SIZE * sizeof(double*));
    
    for (int i = 0; i < SIZE; i++) {
        A[i] = (double*)malloc(SIZE * sizeof(double));
        B[i] = (double*)malloc(SIZE * sizeof(double));
        C[i] = (double*)malloc(SIZE * sizeof(double));
    }
    
    // Initialize matrices
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            A[i][j] = (double)rand() / RAND_MAX;
            B[i][j] = (double)rand() / RAND_MAX;
        }
    }
    
    printf("Matrix multiplication benchmark (%d x %d)\n", SIZE, SIZE);
    print_omp_info();
    
    // Sequential benchmark
    for (int iter = 0; iter < ITERATIONS; iter++) {
        start = omp_get_time();
        matrix_multiply_seq(A, B, C, SIZE);
        end = omp_get_time();
        seq_time += (end - start);
        printf("Sequential iteration %d: %.4f seconds\n", iter + 1, end - start);
    }
    
    // Parallel benchmark
    for (int iter = 0; iter < ITERATIONS; iter++) {
        start = omp_get_time();
        matrix_multiply_parallel(A, B, C, SIZE);
        end = omp_get_time();
        parallel_time += (end - start);
        printf("Parallel iteration %d: %.4f seconds\n", iter + 1, end - start);
    }
    
    printf("\nAverage sequential time: %.4f seconds\n", seq_time / ITERATIONS);
    printf("Average parallel time: %.4f seconds\n", parallel_time / ITERATIONS);
    printf("Speedup: %.2f\n", seq_time / parallel_time);
    
    // Free memory
    for (int i = 0; i < SIZE; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);
    
    return 0;
}
