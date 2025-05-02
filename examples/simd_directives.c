// Example demonstrating OpenMP SIMD directives
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

// Function to demonstrate basic SIMD directive
void vector_ops_simd(float* a, float* b, float* c, int n) {
    double start = omp_get_wtime();
    
    // Standard parallel for
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        c[i] = a[i] * a[i] + b[i];
    }
    
    double middle = omp_get_wtime();
    
    // SIMD-enabled parallel for
    #pragma omp parallel for simd
    for (int i = 0; i < n; i++) {
        c[i] = a[i] * a[i] + b[i];
    }
    
    double end = omp_get_wtime();
    
    printf("Vector operation on %d elements:\n", n);
    printf("  Standard parallel for: %.6f seconds\n", middle - start);
    printf("  SIMD-enabled parallel for: %.6f seconds\n", end - middle);
    printf("  Speedup: %.2fx\n", (middle - start) / (end - middle));
}

// Function demonstrating SIMD with alignment
void simd_alignment_example(float* a, float* b, int n) {
    printf("\nSIMD alignment example:\n");
    double start = omp_get_wtime();
    
    // Aligned memory example with safelen
    #pragma omp simd aligned(a, b:64) safelen(16)
    for (int i = 0; i < n; i++) {
        a[i] = sin(b[i]) * cos(b[i]);
    }
    
    double end = omp_get_wtime();
    printf("  SIMD with alignment directives: %.6f seconds\n", end - start);
    
    // Display a small sample of results
    printf("  Sample results: a[0]=%.4f, a[1]=%.4f, a[2]=%.4f\n", 
           a[0], a[1], a[2]);
}

// Function demonstrating SIMD with multiple reduction operations
void simd_reduction_example(float* data, int n) {
    float sum = 0.0f, min_val = INFINITY, max_val = -INFINITY;
    
    printf("\nSIMD reduction example:\n");
    double start = omp_get_wtime();
    
    // Multiple reduction operations
    #pragma omp simd reduction(+:sum) reduction(min:min_val) reduction(max:max_val)
    for (int i = 0; i < n; i++) {
        sum += data[i];
        min_val = fmin(min_val, data[i]);
        max_val = fmax(max_val, data[i]);
    }
    
    double end = omp_get_wtime();
    
    printf("  SIMD reduction time: %.6f seconds\n", end - start);
    printf("  Results: sum=%.2f, min=%.2f, max=%.2f\n", 
           sum, min_val, max_val);
}

// Function demonstrating collapse clause with SIMD
void simd_collapse_example() {
    const int N = 100;
    const int M = 100;
    float matrix[N][M];
    float sum = 0.0f;
    
    printf("\nSIMD collapse example:\n");
    
    // Initialize matrix
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            matrix[i][j] = (float)(i * j) / (N * M);
        }
    }
    
    double start = omp_get_wtime();
    
    // Collapse nested loops with SIMD
    #pragma omp parallel for simd collapse(2) reduction(+:sum)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            sum += matrix[i][j];
        }
    }
    
    double end = omp_get_wtime();
    
    printf("  SIMD collapse time: %.6f seconds\n", end - start);
    printf("  Sum of matrix elements: %.2f\n", sum);
}

int main() {
    const int SIZE = 10000000;
    
    printf("=== OpenMP SIMD Directives Examples ===\n\n");
    
    // Allocate aligned memory
    float* a = (float*)aligned_alloc(64, SIZE * sizeof(float));
    float* b = (float*)aligned_alloc(64, SIZE * sizeof(float));
    float* c = (float*)aligned_alloc(64, SIZE * sizeof(float));
    
    if (!a || !b || !c) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    // Initialize data
    #pragma omp parallel for
    for (int i = 0; i < SIZE; i++) {
        a[i] = (float)i / SIZE;
        b[i] = (float)(SIZE - i) / SIZE;
        c[i] = 0.0f;
    }
    
    // Run examples
    vector_ops_simd(a, b, c, SIZE);
    simd_alignment_example(a, b, SIZE);
    simd_reduction_example(a, SIZE);
    simd_collapse_example();
    
    // Clean up
    free(a);
    free(b);
    free(c);
    
    return 0;
}