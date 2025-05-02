// Example demonstrating different OpenMP scheduling strategies
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

int main() {
    const int SIZE = 1000000;
    double sum_static = 0.0, sum_dynamic = 0.0, sum_guided = 0.0;
    double start, end;
    
    // Initialize work array with uneven workload
    double* work = (double*)malloc(SIZE * sizeof(double));
    for (int i = 0; i < SIZE; i++) {
        work[i] = i % 100 ? 1.0 : 10000.0; // Every 100th element requires more work
    }
    
    printf("Comparing different scheduling strategies:\n");
    
    // Static scheduling (equal chunks)
    start = omp_get_wtime();
    #pragma omp parallel for reduction(+:sum_static) schedule(static, 1000)
    for (int i = 0; i < SIZE; i++) {
        // Simulate different work amounts
        for (int j = 0; j < work[i]; j++) {
            sum_static += sin(j) * cos(j);
        }
    }
    end = omp_get_wtime();
    printf("Static scheduling: %.4f seconds\n", end - start);
    
    // Dynamic scheduling (work stealing)
    start = omp_get_wtime();
    #pragma omp parallel for reduction(+:sum_dynamic) schedule(dynamic, 1000)
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < work[i]; j++) {
            sum_dynamic += sin(j) * cos(j);
        }
    }
    end = omp_get_wtime();
    printf("Dynamic scheduling: %.4f seconds\n", end - start);
    
    // Guided scheduling (decreasing chunk size)
    start = omp_get_wtime();
    #pragma omp parallel for reduction(+:sum_guided) schedule(guided, 100)
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < work[i]; j++) {
            sum_guided += sin(j) * cos(j);
        }
    }
    end = omp_get_wtime();
    printf("Guided scheduling: %.4f seconds\n", end - start);
    
    printf("\nResults verification (all should be similar):\n");
    printf("Static sum: %.1f\n", sum_static);
    printf("Dynamic sum: %.1f\n", sum_dynamic);
    printf("Guided sum: %.1f\n", sum_guided);
    
    free(work);
    return 0;
}