#ifndef OMP_UTILS_H
#define OMP_UTILS_H

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Helper to measure execution time
static inline double omp_get_time() {
    return omp_get_wtime();
}

// Helper to print parallel region info
static inline void print_omp_info() {
    #pragma omp parallel
    {
        #pragma omp single
        {
            printf("Number of available threads: %d\n", omp_get_num_threads());
            printf("Max threads supported by this implementation: %d\n", omp_get_max_threads());
            printf("Number of available processors: %d\n", omp_get_num_procs());
        }
    }
}

// Set number of threads based on available cores with a cap
static inline void set_optimal_threads() {
    int num_procs = omp_get_num_procs();
    // Use 80% of available processors, minimum 2
    int optimal = (num_procs * 0.8 > 2) ? num_procs * 0.8 : 2;
    omp_set_num_threads(optimal);
}

#endif // OMP_UTILS_H
