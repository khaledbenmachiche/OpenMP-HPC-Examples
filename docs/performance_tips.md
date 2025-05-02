# OpenMP Performance Optimization Guide

## Understanding Parallel Performance

### Amdahl's Law

The maximum speedup you can achieve through parallelization is limited by the serial portion of your code:

```
Speedup = 1 / (S + P/N)
```

Where:
- S = Serial fraction of the code
- P = Parallel fraction of the code
- N = Number of processors

Even with infinite processors, maximum speedup = 1/S

### Overhead Considerations

Every parallel operation involves some overhead:
- Thread creation and destruction
- Synchronization cost
- Memory traffic and cache coherency
- Load imbalance

For maximum performance, the work performed in parallel must be substantial enough to overcome this overhead.

## General Optimization Strategies

### 1. Maximize Parallelism

- Identify and remove serial bottlenecks
- Ensure proper granularity (not too fine, not too coarse)
- Use nested parallelism when appropriate

```c
#pragma omp parallel
{
    // Outer parallel region
    
    #pragma omp parallel for if(n > threshold)
    for (int i = 0; i < n; i++) {
        // Inner parallel region (only created if beneficial)
    }
}
```

### 2. Minimize Data Sharing

- Use private variables when possible
- Promote scalar expansion (private arrays) for temporary data
- Avoid excessive sharing of data between threads

```c
// Less efficient - sharing temp array
#pragma omp parallel for shared(temp, result)
for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
        temp[j] = compute(i, j);  // Potential contention
    }
    result[i] = process(temp, m);
}

// More efficient - private temp array
#pragma omp parallel for private(local_temp)
for (int i = 0; i < n; i++) {
    double local_temp[M];  // Each thread gets its own copy
    for (int j = 0; j < m; j++) {
        local_temp[j] = compute(i, j);
    }
    result[i] = process(local_temp, m);
}
```

### 3. Optimize Loop Construction

- Place parallel directives on outermost possible loop
- Ensure loops have sufficient iterations to warrant parallelization
- Use collapse clause for nested loops with small iteration counts

```c
// Parallelize both loops together
#pragma omp parallel for collapse(2)
for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
        result[i][j] = compute(i, j);
    }
}
```

### 4. Choose Appropriate Scheduling

- `static`: Best when workload is evenly distributed
- `dynamic`: Best for irregular workloads
- `guided`: Good compromise for partially irregular workloads
- `auto`: Let the runtime decide

```c
// For irregular workloads (some iterations take longer than others)
#pragma omp parallel for schedule(dynamic, 16)
for (int i = 0; i < n; i++) {
    result[i] = variable_cost_function(i);
}
```

### 5. Avoid False Sharing

False sharing occurs when threads modify different variables that happen to share the same cache line:

```c
// Problematic - counters may share cache lines
double counters[NUM_THREADS];

#pragma omp parallel
{
    int id = omp_get_thread_num();
    for (int i = 0; i < iterations; i++) {
        counters[id]++;  // May cause false sharing
    }
}

// Better approach - use padding
struct PaddedCounter {
    double value;
    char padding[64 - sizeof(double)];  // Assume 64-byte cache lines
};
struct PaddedCounter counters[NUM_THREADS];
```

### 6. Use Thread-Local Storage

For frequently updated thread-specific data:

```c
#pragma omp threadprivate(my_data)
static int my_data;

#pragma omp parallel
{
    my_data = omp_get_thread_num();  // Each thread has its own copy
    // my_data remains threadprivate across parallel regions
}
```

### 7. Balance Memory Access Patterns

- Ensure stride-1 access for better cache performance
- Place data used together in memory close to each other
- Use data structures that promote locality

```c
// Poor memory access pattern (strided access)
for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
        sum += matrix[j][i];  // Column-wise traversal of row-major array
    }
}

// Better memory access pattern
for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
        sum += matrix[i][j];  // Row-wise traversal of row-major array
    }
}
```

### 8. Minimize Synchronization

- Use atomic operations instead of critical sections when possible
- Use reduction for common patterns
- Avoid unnecessary barriers

```c
// Less efficient
double sum = 0.0;
#pragma omp parallel for
for (int i = 0; i < n; i++) {
    double local_sum = compute(i);
    #pragma omp critical
    {
        sum += local_sum;  // Serialized updates
    }
}

// More efficient
double sum = 0.0;
#pragma omp parallel for reduction(+:sum)
for (int i = 0; i < n; i++) {
    sum += compute(i);  // OpenMP handles reduction efficiently
}
```

## Advanced Optimization Techniques

### NUMA Awareness

On Non-Uniform Memory Access architectures, memory affinity matters:

```c
// First touch initialization - helps with memory locality
#pragma omp parallel for
for (int i = 0; i < n; i++) {
    array[i] = initial_value;  // Each thread initializes its portion
}

// Later work with same thread distribution benefits from locality
#pragma omp parallel for
for (int i = 0; i < n; i++) {
    array[i] = process(array[i]);  // Memory likely local to thread
}
```

### Vectorization with SIMD

Combine threading with vector operations:

```c
#pragma omp parallel for simd
for (int i = 0; i < n; i++) {
    c[i] = a[i] * b[i];
}
```

### Task-Based Optimization

For irregular parallelism:

```c
#pragma omp parallel
{
    #pragma omp single
    {
        process_tree_recursive(root);  // Will create tasks
    }
}

void process_tree_recursive(Node* node) {
    if (node == NULL) return;
    
    process_node(node);
    
    #pragma omp task if(node->depth < CUTOFF_DEPTH)
    process_tree_recursive(node->left);
    
    #pragma omp task if(node->depth < CUTOFF_DEPTH)
    process_tree_recursive(node->right);
}
```

## Measuring and Tuning Performance

### Tools for Analysis

- **Intel VTune Profiler**: Detailed CPU analysis
- **LIKWID**: Hardware counter measurements
- **TAU (Tuning and Analysis Utilities)**: Performance system
- **Valgrind/Cachegrind**: Memory access analysis
- **OpenMP's own timing functions**:

```c
double start = omp_get_wtime();
// Code to measure
double end = omp_get_wtime();
printf("Time: %f seconds\n", end - start);
```

### Tuning Methodology

1. Establish baseline sequential performance
2. Find hotspots and parallelize them
3. Measure scaling with different thread counts
4. Identify and remove synchronization bottlenecks
5. Address memory access patterns
6. Tune scheduling policies
7. Consider algorithmic improvements

## Environment Variables for Performance

- `OMP_NUM_THREADS`: Set number of threads
- `OMP_PROC_BIND`: Control thread affinity (close, spread, master)
- `OMP_PLACES`: Define where threads can be placed (cores, sockets)
- `OMP_SCHEDULE`: Default scheduling policy
- `OMP_STACKSIZE`: Thread stack size

```bash
# Example: Set thread affinity to keep threads close together
export OMP_PROC_BIND=close
export OMP_PLACES=cores
export OMP_NUM_THREADS=16
./my_openmp_program
```