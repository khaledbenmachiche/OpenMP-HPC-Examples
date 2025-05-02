#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "../include/omp_utils.h"

/**
 * Parallel implementation of array reduction
 * @param arr Input array
 * @param size Array size
 * @param initial Initial value for reduction
 * @param op Operation: 0=sum, 1=product, 2=max, 3=min
 * @return Result of reduction operation
 */
double parallel_reduce(const double* arr, int size, double initial, int op) {
    double result = initial;
    
    switch(op) {
        case 0: // sum
            #pragma omp parallel for reduction(+:result)
            for (int i = 0; i < size; i++) {
                result += arr[i];
            }
            break;
            
        case 1: // product
            #pragma omp parallel for reduction(*:result)
            for (int i = 0; i < size; i++) {
                result *= arr[i];
            }
            break;
            
        case 2: // max
            #pragma omp parallel for reduction(max:result)
            for (int i = 0; i < size; i++) {
                if (arr[i] > result) result = arr[i];
            }
            break;
            
        case 3: // min
            #pragma omp parallel for reduction(min:result)
            for (int i = 0; i < size; i++) {
                if (arr[i] < result) result = arr[i];
            }
            break;
    }
    
    return result;
}

/**
 * Parallel implementation of array transformation (map)
 * @param in Input array
 * @param out Output array
 * @param size Array size
 * @param func Function pointer: double (*func)(double)
 */
void parallel_transform(const double* in, double* out, int size, double (*func)(double)) {
    #pragma omp parallel for
    for (int i = 0; i < size; i++) {
        out[i] = func(in[i]);
    }
}

/**
 * Parallel implementation of array sorting (mergesort)
 */
void merge(double* arr, double* temp, int left, int mid, int right) {
    int i = left;
    int j = mid + 1;
    int k = left;
    
    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j])
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
    }
    
    while (i <= mid)
        temp[k++] = arr[i++];
    
    while (j <= right)
        temp[k++] = arr[j++];
    
    for (i = left; i <= right; i++)
        arr[i] = temp[i];
}

void mergesort_serial(double* arr, double* temp, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergesort_serial(arr, temp, left, mid);
        mergesort_serial(arr, temp, mid + 1, right);
        merge(arr, temp, left, mid, right);
    }
}

void mergesort_parallel(double* arr, double* temp, int left, int right, int depth) {
    if (depth <= 0 || (right - left) <= 1000) {
        // Switch to serial algorithm when we've spawned enough tasks or problem is small
        mergesort_serial(arr, temp, left, right);
        return;
    }
    
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        #pragma omp task
        mergesort_parallel(arr, temp, left, mid, depth-1);
        
        #pragma omp task
        mergesort_parallel(arr, temp, mid + 1, right, depth-1);
        
        #pragma omp taskwait
        merge(arr, temp, left, mid, right);
    }
}

void parallel_sort(double* arr, int size) {
    double* temp = (double*)malloc(size * sizeof(double));
    if (!temp) return;
    
    int max_depth = omp_get_max_threads();
    
    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            mergesort_parallel(arr, temp, 0, size - 1, max_depth);
        }
    }
    
    free(temp);
}
