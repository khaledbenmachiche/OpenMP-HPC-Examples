# Getting Started with OpenMP

## Introduction

OpenMP (Open Multi-Processing) is an API that enables portable, scalable parallel programming on shared memory systems. This guide will help you quickly get up and running with OpenMP programming.

## Prerequisites

- A C/C++ compiler with OpenMP support (GCC, Clang, Intel compiler, etc.)
- Basic understanding of C/C++ programming
- Your compiler's OpenMP flag (typically `-fopenmp` for GCC/Clang)

## Installation

Most modern C/C++ compilers already include OpenMP support. You simply need to enable it during compilation:

### GCC/Clang
```bash
gcc -fopenmp program.c -o program
```

### Intel Compiler
```bash
icc -qopenmp program.c -o program
```

## Your First OpenMP Program

Create a file named `hello_openmp.c` with the following content:

```c
#include <stdio.h>
#include <omp.h>

int main() {
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int total_threads = omp_get_num_threads();
        
        printf("Hello from thread %d of %d!\n", thread_id, total_threads);
    }
    
    return 0;
}
```

Compile and run the program:

```bash
gcc -fopenmp hello_openmp.c -o hello_openmp
./hello_openmp
```

By default, OpenMP will use as many threads as there are logical cores on your system.

## Controlling Thread Count

You can control the number of threads in several ways:

### Using environment variables
```bash
export OMP_NUM_THREADS=4
./hello_openmp
```

### Using OpenMP functions
```c
#include <stdio.h>
#include <omp.h>

int main() {
    omp_set_num_threads(4);  // Request 4 threads
    
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int total_threads = omp_get_num_threads();
        
        printf("Hello from thread %d of %d!\n", thread_id, total_threads);
    }
    
    return 0;
}
```

## Where to Go Next

- Check out our [OpenMP Constructs Guide](./openmp_constructs.md) to learn about parallel loops and work distribution
- See practical examples in the [examples directory](../examples/)
- Review our [performance optimization tips](./performance_tips.md)
- Learn about [synchronization mechanisms](./synchronization.md) to avoid race conditions