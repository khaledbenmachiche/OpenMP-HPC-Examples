#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include "../include/omp_utils.h"
#include "../include/parallel_algorithms.h"

#define ARRAY_SIZE 10000000
#define EPSILON 1e-10

// Test function for parallel_transform
double square_root(double x) {
    return sqrt(x);
}

int test_reduction() {
    printf("\n=== Testing Parallel Reduction ===\n");
    double *arr = (double*)malloc(ARRAY_SIZE * sizeof(double));
    
    // Initialize array
    srand(time(NULL));
    double expected_sum = 0.0;
    double expected_product = 1.0;
    double expected_max = -INFINITY;
    double expected_min = INFINITY;
    
    for (int i = 0; i < ARRAY_SIZE; i++) {
        arr[i] = (double)rand() / RAND_MAX * 10.0;
        expected_sum += arr[i];
        expected_product *= arr[i];
        if (arr[i] > expected_max) expected_max = arr[i];
        if (arr[i] < expected_min) expected_min = arr[i];
    }
    
    // Test sum reduction
    double sum_result = parallel_reduce(arr, ARRAY_SIZE, 0.0, 0);
    printf("Sum: expected %.2f, got %.2f -> %s\n", 
           expected_sum, sum_result, 
           fabs(expected_sum - sum_result) < EPSILON ? "PASS" : "FAIL");
    
    // Test max reduction
    double max_result = parallel_reduce(arr, ARRAY_SIZE, -INFINITY, 2);
    printf("Max: expected %.2f, got %.2f -> %s\n", 
           expected_max, max_result, 
           fabs(expected_max - max_result) < EPSILON ? "PASS" : "FAIL");
    
    // Test min reduction
    double min_result = parallel_reduce(arr, ARRAY_SIZE, INFINITY, 3);
    printf("Min: expected %.2f, got %.2f -> %s\n", 
           expected_min, min_result, 
           fabs(expected_min - min_result) < EPSILON ? "PASS" : "FAIL");
    
    free(arr);
    return 0;
}

int test_transform() {
    printf("\n=== Testing Parallel Transform ===\n");
    double *in = (double*)malloc(ARRAY_SIZE * sizeof(double));
    double *out = (double*)malloc(ARRAY_SIZE * sizeof(double));
    double *expected = (double*)malloc(ARRAY_SIZE * sizeof(double));
    
    // Initialize array
    srand(time(NULL) + 100);
    for (int i = 0; i < ARRAY_SIZE; i++) {
        in[i] = (double)rand() / RAND_MAX * 100.0;
        expected[i] = square_root(in[i]);
    }
    
    // Test parallel transform
    parallel_transform(in, out, ARRAY_SIZE, square_root);
    
    // Verify results
    int errors = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (fabs(out[i] - expected[i]) > EPSILON) {
            errors++;
        }
    }
    
    printf("Transform: %d errors out of %d -> %s\n", 
           errors, ARRAY_SIZE, errors == 0 ? "PASS" : "FAIL");
    
    free(in);
    free(out);
    free(expected);
    return 0;
}

int test_sort() {
    printf("\n=== Testing Parallel Sort ===\n");
    const int sizes[] = {1000, 10000, 100000};
    const int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int s = 0; s < num_sizes; s++) {
        int size = sizes[s];
        double *arr = (double*)malloc(size * sizeof(double));
        
        // Initialize with random values
        srand(time(NULL) + s);
        for (int i = 0; i < size; i++) {
            arr[i] = (double)rand() / RAND_MAX * 1000.0;
        }
        
        // Sort the array
        double start = omp_get_time();
        parallel_sort(arr, size);
        double end = omp_get_time();
        
        // Verify sorting
        int is_sorted = 1;
        for (int i = 1; i < size; i++) {
            if (arr[i] < arr[i-1]) {
                is_sorted = 0;
                break;
            }
        }
        
        printf("Sort %d elements: %.4f seconds -> %s\n", 
               size, end - start, is_sorted ? "PASS" : "FAIL");
        
        free(arr);
    }
    
    return 0;
}

int main() {
    printf("Running tests for parallel algorithms\n");
    print_omp_info();
    
    test_reduction();
    test_transform();
    test_sort();
    
    printf("\nAll tests completed.\n");
    return 0;
}
