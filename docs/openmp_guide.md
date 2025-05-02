# OpenMP Programming Guide

## Introduction

OpenMP (Open Multi-Processing) is an API that supports multi-platform shared-memory parallel programming in C, C++, and Fortran. It consists of compiler directives, library routines, and environment variables that influence runtime behavior.

This guide provides an overview of the OpenMP API. For more detailed information on specific aspects, please refer to the specialized guides in the documentation directory.

## OpenMP Documentation Structure

This documentation is organized into the following sections:

- [Getting Started](getting_started.md): First steps with OpenMP, installation and basic concepts
- [OpenMP Constructs](openmp_constructs.md): Detailed information on parallel regions, work-sharing and synchronization constructs
- [Data Sharing](data_sharing.md): How to control variable sharing between threads
- [Synchronization](synchronization.md): Coordination between threads
- [Performance Tips](performance_tips.md): Guidelines for optimizing OpenMP programs

## Basic Concepts

### Parallel Regions

The fundamental building block in OpenMP is the parallel region:

```c
#pragma omp parallel
{
    // Code here is executed by multiple threads
    int thread_id = omp_get_thread_num();
    int total_threads = omp_get_num_threads();
    
    printf("Hello from thread %d of %d!\n", thread_id, total_threads);
}
```

### Work-Sharing Constructs

OpenMP provides several constructs for distributing work among threads:

1. **Parallel For Loop**:
```c
#pragma omp parallel for
for (int i = 0; i < n; i++) {
    // Loop iterations are distributed among threads
    process(i);
}
```

2. **Sections**:
```c
#pragma omp parallel sections
{
    #pragma omp section
    {
        task_a();
    }
    
    #pragma omp section
    {
        task_b();
    }
}
```

3. **Task-Based Parallelism**:
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
}
```

See [OpenMP Constructs](openmp_constructs.md) for more detailed information.

### Data Sharing Clauses

OpenMP provides clauses to control how variables are shared:

- **shared(var)**: All threads access the same variable
- **private(var)**: Each thread has its own independent copy
- **firstprivate(var)**: Like private, but initialized with the value from outside
- **lastprivate(var)**: Like private, but the final value is copied out
- **reduction(op:var)**: Threads maintain private copies that are combined

Example:
```c
int sum = 0;
#pragma omp parallel for reduction(+:sum) private(temp)
for (int i = 0; i < n; i++) {
    temp = array[i] * 2;
    sum += temp;
}
```

See [Data Sharing](data_sharing.md) for a complete guide.

### Synchronization

OpenMP provides several mechanisms for thread coordination:

- **barrier**: All threads wait at this point
```c
#pragma omp barrier
```

- **critical**: Only one thread executes at a time
```c
#pragma omp critical
{
    // Only one thread can be here at a time
}
```

- **atomic**: Ensures atomic update of a memory location
```c
#pragma omp atomic
counter++;
```

See [Synchronization](synchronization.md) for more details.

## Environment Variables

OpenMP behavior can be controlled via environment variables:

- **OMP_NUM_THREADS**: Set number of threads
  ```bash
  export OMP_NUM_THREADS=4
  ```

- **OMP_SCHEDULE**: Default schedule type and chunk size
  ```bash
  export OMP_SCHEDULE="dynamic,16"
  ```

- **OMP_PROC_BIND**: Thread affinity policy (close, spread, master)
  ```bash
  export OMP_PROC_BIND=close
  ```

- **OMP_PLACES**: Where threads can execute (threads, cores, sockets)
  ```bash
  export OMP_PLACES=cores
  ```

## Runtime Library Functions

OpenMP provides a runtime library with functions to:

- Control execution: `omp_set_num_threads()`, `omp_get_num_threads()`
- Query environment: `omp_get_thread_num()`, `omp_get_max_threads()`
- Timing: `omp_get_wtime()`
- Locks: `omp_init_lock()`, `omp_set_lock()`, etc.

Example:
```c
#include <omp.h>

int main() {
    double start = omp_get_wtime();
    
    omp_set_num_threads(4);
    #pragma omp parallel
    {
        // Parallel work
    }
    
    double end = omp_get_wtime();
    printf("Time: %f seconds\n", end - start);
    
    return 0;
}
```

## Compilation

To compile an OpenMP program:

- **GCC/Clang**: `-fopenmp`
  ```bash
  gcc -fopenmp program.c -o program
  ```

- **Intel Compiler**: `-qopenmp`
  ```bash
  icc -qopenmp program.c -o program
  ```

## Examples

Check out the examples directory for practical implementations:

- [hello_world.c](../examples/hello_world.c): Basic parallel region
- [matrix_multiply.c](../benchmarks/matrix_multiply.c): Parallel matrix multiplication

## Performance Considerations

For optimizing OpenMP performance, see our detailed [Performance Tips](performance_tips.md) guide, which covers:

1. **Choosing appropriate parallelism**: Task vs data parallelism
2. **Load balancing**: Dynamic vs static scheduling
3. **Data locality**: Memory access patterns and cache optimization
4. **Minimizing synchronization**: Reducing thread coordination overhead
5. **Advanced techniques**: SIMD, task-based parallelism, thread affinity

## Further Resources

- [OpenMP Official Website](https://www.openmp.org/)
- [OpenMP API Specification](https://www.openmp.org/specifications/)
- [OpenMP Reference Guide](https://www.openmp.org/resources/refguides/)
- Books:
  - "Using OpenMP: Portable Shared Memory Parallel Programming" by Barbara Chapman, et al.
  - "Parallel Programming in C with MPI and OpenMP" by Michael J. Quinn
