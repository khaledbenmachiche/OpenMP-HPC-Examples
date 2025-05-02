// Example demonstrating OpenMP data sharing clauses
#include <stdio.h>
#include <omp.h>

int main() {
    int shared_var = 0;
    int private_result = 0;
    int firstprivate_var = 100;
    int lastprivate_var = 0;
    int reduction_sum = 0;
    
    printf("Before parallel region: shared_var=%d, firstprivate_var=%d\n", 
           shared_var, firstprivate_var);
    
    #pragma omp parallel num_threads(4) \
            shared(shared_var) \
            private(private_result) \
            firstprivate(firstprivate_var) \
            lastprivate(lastprivate_var) \
            reduction(+:reduction_sum)
    {
        int thread_id = omp_get_thread_num();
        
        // Shared variable - all threads access the same memory location
        #pragma omp critical
        {
            shared_var++;
            printf("Thread %d: Incremented shared_var to %d\n", thread_id, shared_var);
        }
        
        // Private variable - each thread has its own copy
        private_result = thread_id * 10;
        printf("Thread %d: private_result = %d\n", thread_id, private_result);
        
        // Firstprivate - initialized with value from outside the parallel region
        printf("Thread %d: firstprivate_var = %d\n", thread_id, firstprivate_var);
        firstprivate_var += thread_id; // This won't affect the original variable
        
        // Lastprivate - the value from the last logical iteration is retained
        #pragma omp for lastprivate(lastprivate_var)
        for (int i = 0; i < 16; i++) {
            lastprivate_var = i + 100 + thread_id;
        }
        
        // Reduction - combine thread-private copies with the specified operator
        #pragma omp for reduction(+:reduction_sum)
        for (int i = 1; i <= 100; i++) {
            reduction_sum += i;
        }
    }
    
    printf("\nAfter parallel region:\n");
    printf("  shared_var = %d (expected to be 4)\n", shared_var);
    printf("  private_result = %d (unchanged from initialization)\n", private_result);
    printf("  firstprivate_var = %d (unchanged from initialization)\n", firstprivate_var);
    printf("  lastprivate_var = %d (value from the thread that executed the last iteration)\n", 
           lastprivate_var);
    printf("  reduction_sum = %d (sum of 1 to 100 = 5050)\n", reduction_sum);
    
    return 0;
}