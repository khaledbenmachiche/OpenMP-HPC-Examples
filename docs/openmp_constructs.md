# OpenMP Constructs Guide

This guide provides a detailed reference for the major OpenMP constructs that form the foundation of OpenMP programming.

## Parallel Regions

### Basic Parallel Region

```c
#pragma omp parallel
{
    // Code executed by all threads in parallel
}
```

This creates a team of threads, with each thread executing the enclosed code block.

### Parallel Region with Clauses

```c
#pragma omp parallel num_threads(4) if(n > threshold) private(x) shared(y)
{
    // Parallel region with specified number of threads and data sharing
}
```

Common clauses:
- `num_threads(n)`: Request specific number of threads
- `if(condition)`: Only parallelize if condition is true
- `private(var-list)`: Give each thread its own copy of variables
- `shared(var-list)`: Share variables among threads
- `default(shared|none)`: Set default variable sharing
- `firstprivate(var-list)`: Initialize private variables from shared ones
- `reduction(op:var-list)`: Perform reduction operations

## Work-Sharing Constructs

### For Loop

```c
#pragma omp parallel
{
    #pragma omp for
    for (int i = 0; i < n; i++) {
        // Loop iterations distributed among threads
    }
}

// Combined form
#pragma omp parallel for
for (int i = 0; i < n; i++) {
    // Loop iterations distributed among threads
}
```

Clauses specific to for:
- `schedule(type[,chunk])`: How iterations are divided
  - `static`: Blocks of iterations are assigned to threads in round-robin
  - `dynamic`: Threads take iterations as they complete previous work
  - `guided`: Like dynamic, but chunk size decreases over time
  - `auto`: Implementation decides
  - `runtime`: Determined by environment variable
- `collapse(n)`: Specify how many nested loops to parallelize
- `ordered`: Iterations executed in original sequential order
- `nowait`: Skip implicit barrier at end of construct

### Sections

```c
#pragma omp parallel
{
    #pragma omp sections
    {
        #pragma omp section
        {
            // Code for first section
        }
        
        #pragma omp section
        {
            // Code for second section
        }
        
        // Add more sections as needed
    }
}

// Combined form
#pragma omp parallel sections
{
    #pragma omp section
    {
        // Code for first section
    }
    
    #pragma omp section
    {
        // Code for second section
    }
}
```

Common clauses:
- `private(var-list)`, `firstprivate(var-list)`, `lastprivate(var-list)`
- `reduction(op:var-list)`
- `nowait`

### Single

```c
#pragma omp parallel
{
    // Code executed by all threads
    
    #pragma omp single
    {
        // Executed by only one thread
        // Other threads wait at implicit barrier
    }
    
    // Code executed by all threads
}
```

Common clauses:
- `private(var-list)`, `firstprivate(var-list)`
- `nowait`: Other threads don't wait at the end

### Master

```c
#pragma omp parallel
{
    // Code executed by all threads
    
    #pragma omp master
    {
        // Executed only by the master thread
        // NO implicit barrier - other threads continue
    }
    
    // Code executed by all threads
}
```

No clauses allowed for master directive.

### Critical

```c
#pragma omp parallel
{
    // Code executed by all threads
    
    #pragma omp critical [(name)]
    {
        // Only one thread executes this block at a time
    }
}
```

Optional name parameter allows multiple independent critical sections.

### Barrier

```c
#pragma omp parallel
{
    // Code executed by all threads
    
    // Each thread does its part of work
    do_work_part1();
    
    #pragma omp barrier
    // All threads synchronize here
    
    // Each thread continues with next part
    do_work_part2();
}
```

No clauses allowed for barrier directive.

### Atomic

```c
#pragma omp parallel
{
    #pragma omp atomic
    counter++;
    
    // Other atomic operations
    #pragma omp atomic read
    value = x;
    
    #pragma omp atomic write
    x = value;
    
    #pragma omp atomic update
    x += 1;
    
    #pragma omp atomic capture
    {old_value = x; x++;}
}
```

## Task Parallelism

### Basic Tasks

```c
#pragma omp parallel
{
    #pragma omp single
    {
        for (node = head; node != NULL; node = node->next) {
            #pragma omp task
            process_node(node);
        }
    }
    // Implicit barrier: all tasks complete here
}
```

Common task clauses:
- `if(condition)`: Create task only if condition is true
- `untied`: Task may be resumed by any thread
- `mergeable`: Merged with ancestor if single-threaded
- `private(var-list)`, `firstprivate(var-list)`
- `shared(var-list)`, `default(shared|none)`

### Task Synchronization

```c
#pragma omp parallel
{
    #pragma omp single
    {
        #pragma omp task
        {
            // Task A
        }
        
        #pragma omp task
        {
            // Task B
        }
        
        #pragma omp taskwait
        // Wait for tasks A and B to complete
        
        #pragma omp task
        {
            // Task C (only starts after A and B)
        }
    }
}
```

### Task Dependencies

```c
#pragma omp parallel
{
    #pragma omp single
    {
        #pragma omp task depend(out: x)
        {
            compute_x();
        }
        
        #pragma omp task depend(out: y)
        {
            compute_y();
        }
        
        #pragma omp task depend(in: x, y) depend(out: z)
        {
            // Only executes after x and y are computed
            z = combine(x, y);
        }
    }
}
```

Dependency types:
- `depend(in: var-list)`: Task depends on the variables being computed
- `depend(out: var-list)`: Task computes the variables
- `depend(inout: var-list)`: Task both reads and modifies variables

## SIMD Constructs

### Basic SIMD

```c
#pragma omp simd
for (int i = 0; i < n; i++) {
    c[i] = a[i] + b[i];
}

// Combined with parallel
#pragma omp parallel for simd
for (int i = 0; i < n; i++) {
    c[i] = a[i] + b[i];
}
```

Common SIMD clauses:
- `safelen(length)`: Maximum vector length for safe operations
- `simdlen(length)`: Preferred vector length
- `aligned(var-list[:alignment])`: Specify data alignment
- `private(var-list)`, `lastprivate(var-list)`, `linear(var-list[:step])`
- `reduction(op:var-list)`

## Teams and Distribute Constructs

For accelerators and coprocessor offloading:

```c
#pragma omp teams
{
    // Code executed by the initial thread of each team
    
    #pragma omp distribute
    for (int i = 0; i < n; i++) {
        // Loop iterations distributed among teams
        
        #pragma omp parallel for
        for (int j = 0; j < m; j++) {
            // Distributed among threads in each team
        }
    }
}

// Combined form
#pragma omp teams distribute parallel for
for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
        compute(i, j);
    }
}
```

## Target Constructs (Offloading)

```c
#pragma omp target map(to: a[0:n]) map(from: b[0:n])
{
    // Code executed on the device (e.g., GPU)
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        b[i] = process(a[i]);
    }
}
```

Common clauses:
- `device(device-num)`: Target specific device
- `map(map-type: var-list)`: Data mapping
  - `map-type` can be `to`, `from`, `tofrom`, `alloc`, `release`, `delete`
- `if(condition)`: Conditional offloading
- `private(var-list)`, `firstprivate(var-list)`, `defaultmap(category:behavior)`

## Nested Parallelism

```c
#pragma omp parallel
{
    // Outer parallel region
    
    #pragma omp parallel
    {
        // Nested parallel region
    }
}
```

Control with:
- `omp_set_nested(1)`: Enable nested parallelism
- `omp_set_max_active_levels(n)`: Set maximum nesting depth

## Example: Combining Multiple Constructs

Here's an example showing multiple OpenMP constructs working together:

```c
#include <omp.h>
#include <stdio.h>

void process_matrix(double** matrix, int rows, int cols) {
    // Allocate result array per team
    double* team_results;
    
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        
        // Single thread allocates team_results
        #pragma omp single
        {
            team_results = (double*)malloc(num_threads * sizeof(double));
            printf("Allocated memory for %d team results\n", num_threads);
        }
        
        // Initialize local result
        double local_result = 0.0;
        
        // Distribute work with dynamic scheduling
        #pragma omp for schedule(dynamic, 16)
        for (int i = 0; i < rows; i++) {
            // Use SIMD for inner loop
            #pragma omp simd reduction(+:local_result)
            for (int j = 0; j < cols; j++) {
                local_result += matrix[i][j];
            }
            
            // Occasionally create tasks for special processing
            if (i % 100 == 0) {
                #pragma omp task
                {
                    printf("Special processing for row %d by thread %d\n", 
                           i, omp_get_thread_num());
                    // Do special processing
                }
            }
        }
        
        // Store local result
        team_results[thread_id] = local_result;
        
        // Wait for all tasks to complete
        #pragma omp barrier
        
        // Reduce to final result
        #pragma omp single
        {
            double final_result = 0.0;
            for (int t = 0; t < num_threads; t++) {
                final_result += team_results[t];
            }
            printf("Final result: %f\n", final_result);
            free(team_results);
        }
    }
}
```

## Best Practices

1. **Start simple**: Begin with parallel regions and simple work-sharing constructs
2. **Use combined directives** when appropriate to make code cleaner
3. **Carefully plan data sharing**: Use `default(none)` to force explicit handling
4. **Minimize synchronization**: Use appropriate constructs to avoid unnecessary barriers
5. **Use tasks** for irregular parallelism
6. **Test with different thread counts** to ensure correctness
7. **Leverage SIMD** when possible for vectorization
8. **Consider nested parallelism** for hierarchical problems
9. **Use newer OpenMP constructs** for better performance on modern hardware