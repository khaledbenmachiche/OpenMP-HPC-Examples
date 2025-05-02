// Example demonstrating OpenMP task-based parallelism with dependencies
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>  // for sleep function

void process_file(const char* filename) {
    double start = omp_get_wtime();
    int file_id = 0;
    sscanf(filename, "file_%d.dat", &file_id);
    
    int data_size = 0;
    int* parsed_data = NULL;
    int* processed_data = NULL;
    char* report = NULL;
    
    printf("Starting processing pipeline for %s\n", filename);
    
    #pragma omp parallel
    {
        #pragma omp single
        {
            // Stage 1: Read data task
            #pragma omp task depend(out:data_size, parsed_data)
            {
                printf("Thread %d: Reading data from %s\n", 
                       omp_get_thread_num(), filename);
                
                // Simulate reading data
                data_size = file_id * 100;
                parsed_data = (int*)malloc(data_size * sizeof(int));
                
                for (int i = 0; i < data_size; i++) {
                    parsed_data[i] = i % 10;
                }
                
                sleep(1);  // Simulate I/O time
                printf("Thread %d: Finished reading %d data points\n", 
                       omp_get_thread_num(), data_size);
            }
            
            // Stage 2: Process data task (depends on read task)
            #pragma omp task depend(in:parsed_data) depend(out:processed_data)
            {
                printf("Thread %d: Processing data from %s\n", 
                       omp_get_thread_num(), filename);
                
                processed_data = (int*)malloc(data_size * sizeof(int));
                
                // Simulate data processing
                for (int i = 0; i < data_size; i++) {
                    processed_data[i] = parsed_data[i] * 2;
                }
                
                sleep(1);  // Simulate processing time
                printf("Thread %d: Finished processing data\n", 
                       omp_get_thread_num());
            }
            
            // Stage 3: Generate report (depends on processing task)
            #pragma omp task depend(in:processed_data) depend(out:report)
            {
                printf("Thread %d: Generating report for %s\n", 
                       omp_get_thread_num(), filename);
                
                // Simulate report generation
                int sum = 0;
                for (int i = 0; i < data_size; i++) {
                    sum += processed_data[i];
                }
                
                report = (char*)malloc(100);
                sprintf(report, "File %s: %d items, sum=%d", filename, data_size, sum);
                
                sleep(1);  // Simulate report generation time
                printf("Thread %d: Report generated\n", 
                       omp_get_thread_num());
            }
            
            // Stage 4: Output results (depends on report task)
            #pragma omp task depend(in:report)
            {
                printf("Thread %d: Saving report to disk: %s\n", 
                       omp_get_thread_num(), report);
                
                // Simulate saving to disk
                sleep(1);  // Simulate I/O time
                printf("Thread %d: Report saved: %s\n", 
                       omp_get_thread_num(), report);
            }
        }
    }
    
    // Clean up
    free(parsed_data);
    free(processed_data);
    free(report);
    
    double end = omp_get_wtime();
    printf("Completed processing %s in %.4f seconds\n", filename, end - start);
}

int main() {
    const char* files[] = {"file_1.dat", "file_2.dat", "file_3.dat"};
    
    // Process multiple files concurrently
    #pragma omp parallel
    {
        #pragma omp single
        {
            for (int i = 0; i < 3; i++) {
                #pragma omp task
                process_file(files[i]);
            }
        }
    }
    
    return 0;
}