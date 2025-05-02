// Example demonstrating OpenMP nested parallelism and thread affinity
#include <stdio.h>
#include <omp.h>
#include <unistd.h> // For sleep

// Function to demonstrate the basics of nested parallelism
void demonstrate_nested_parallelism() {
    printf("--- Nested Parallelism Example ---\n");
    
    // Enable nested parallelism
    omp_set_nested(1);
    omp_set_dynamic(0); // Disable dynamic adjustment of threads
    
    printf("Nested parallelism is %s\n", 
           omp_get_nested() ? "enabled" : "disabled");
    
    // Create a team of 3 threads
    #pragma omp parallel num_threads(3)
    {
        int outer_id = omp_get_thread_num();
        int outer_team_size = omp_get_num_threads();
        
        printf("Outer thread %d/%d starting\n", outer_id, outer_team_size);
        
        // Each thread in the outer team creates a team of 2 threads
        #pragma omp parallel num_threads(2)
        {
            int inner_id = omp_get_thread_num();
            int inner_team_size = omp_get_num_threads();
            
            printf("  Inner thread %d/%d within outer thread %d\n", 
                   inner_id, inner_team_size, outer_id);
            
            // Do some work
            double result = 0.0;
            for (int i = 0; i < 10000000; i++) {
                result += sin((double)i / 10000000.0);
            }
        }
        
        printf("Outer thread %d completed its inner parallel region\n", outer_id);
    }
}

// Function to demonstrate thread affinity settings
void demonstrate_thread_affinity() {
    printf("\n--- Thread Affinity Example ---\n");
    printf("Note: Set OMP_DISPLAY_AFFINITY=true environment variable\n");
    printf("      for detailed affinity information at runtime\n\n");
    
    // We'll show how to use affinity within the code
    
    // Example 1: Close affinity (threads close to each other)
    printf("Example with close affinity policy:\n");
    omp_set_num_threads(4);
    
    #pragma omp parallel proc_bind(close)
    {
        int id = omp_get_thread_num();
        
        // Get the place number for this thread
        int place_num = omp_get_place_num();
        
        // Get the number of procs in the place
        int place_size = omp_get_place_num_procs(place_num);
        
        // Display this thread's affinity
        printf("Thread %d running with close affinity on place %d with %d processors\n", 
               id, place_num, place_size);
        
        // Do some work
        sleep(1);
    }
    
    // Example 2: Spread affinity (threads distributed)
    printf("\nExample with spread affinity policy:\n");
    
    #pragma omp parallel proc_bind(spread)
    {
        int id = omp_get_thread_num();
        
        // Get the place number for this thread
        int place_num = omp_get_place_num();
        
        // Get the number of procs in the place
        int place_size = omp_get_place_num_procs(place_num);
        
        printf("Thread %d running with spread affinity on place %d with %d processors\n", 
               id, place_num, place_size);
        
        // Do some work
        sleep(1);
    }
}

// Function to demonstrate thread affinity with nested parallel regions
void demonstrate_nested_affinity() {
    printf("\n--- Nested Parallelism with Thread Affinity ---\n");
    
    // Enable nested parallelism
    omp_set_nested(1);
    omp_set_dynamic(0);
    
    #pragma omp parallel num_threads(2) proc_bind(spread)
    {
        int outer_id = omp_get_thread_num();
        int place_num = omp_get_place_num();
        
        printf("Outer thread %d on place %d\n", outer_id, place_num);
        
        #pragma omp parallel num_threads(2) proc_bind(close)
        {
            int inner_id = omp_get_thread_num();
            int inner_place = omp_get_place_num();
            
            printf("  Inner thread %d from outer thread %d on place %d\n", 
                   inner_id, outer_id, inner_place);
            
            // Do some work
            sleep(1);
        }
    }
}

int main() {
    printf("=== OpenMP Nested Parallelism and Thread Affinity Examples ===\n\n");
    
    demonstrate_nested_parallelism();
    
    // Check if affinity is supported before running affinity examples
    if (omp_get_num_places() > 0) {
        demonstrate_thread_affinity();
        demonstrate_nested_affinity();
    } else {
        printf("\nThread affinity not supported on this system.\n");
        printf("Try setting OMP_PLACES=cores before running for affinity support.\n");
    }
    
    return 0;
}