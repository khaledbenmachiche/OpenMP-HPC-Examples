# OpenMP Data Sharing Guide

Data sharing is a critical aspect of OpenMP programming as it determines how variables are accessed and modified across threads. This guide explains the different data sharing attributes and best practices for managing memory access in parallel regions.

## Data Sharing Attributes

OpenMP provides several data sharing attributes to control how variables are shared among threads:

### Shared Variables

When a variable is declared as `shared`, all threads access the same memory location. This is useful for read-only data or variables that need to be updated cooperatively.

```c
int x = 10;
#pragma omp parallel shared(x)
{
    // All threads see and modify the same variable x
    x = x + 1;  // Race condition!
}
```

**Warning**: Writing to shared variables without proper synchronization causes race conditions.

### Private Variables

When a variable is declared as `private`, each thread gets its own copy of the variable. The private copy is not initialized and its value after the parallel region is undefined.

```c
int i = 10;
#pragma omp parallel private(i)
{
    i = omp_get_thread_num();  // Each thread has its own i
    printf("Thread %d has i = %d\n", omp_get_thread_num(), i);
}
// Value of i here is undefined
```

### Firstprivate Variables

Like `private` but initial values are copied from the original variables when entering the parallel region.

```c
int i = 10;
#pragma omp parallel firstprivate(i)
{
    // Each thread has its own i, initialized to 10
    i += omp_get_thread_num();
    printf("Thread %d has i = %d\n", omp_get_thread_num(), i);
}
// Value of i here is still 10
```

### Lastprivate Variables

Like `private` but the value from the last iteration (for loops) or section (for sections) is copied back to the original variable.

```c
int i;
#pragma omp parallel
{
    #pragma omp for lastprivate(i)
    for (i = 0; i < 100; i++) {
        // Do work
    }
}
// i now equals 99 (value from the last iteration)
```

### Default Clause

The `default` clause sets the default data-sharing attributes for variables in a parallel region:

```c
#pragma omp parallel default(shared)
{
    // All variables referenced are shared by default
}

#pragma omp parallel default(none) shared(x) private(y)
{
    // Forces explicit declaration of all variables
}
```

Options:
- `default(shared)`: All variables are shared by default
- `default(none)`: Forces explicit declaration of every variable
- `default(firstprivate)`: All variables are firstprivate by default (OpenMP 5.0+)

## Reduction Variables

The `reduction` clause provides a safe way to update shared variables:

```c
int sum = 0;
#pragma omp parallel for reduction(+:sum)
for (int i = 0; i < n; i++) {
    sum += array[i];
}
// sum contains the total
```

Each thread gets a private copy of the reduction variable, initialized according to the operation (e.g., 0 for +, 1 for *). At the end, all private copies are combined using the specified operator.

Supported operators: `+`, `-`, `*`, `&`, `|`, `^`, `&&`, `||`, `min`, `max`

## Linear Variables

The `linear` clause is used primarily with SIMD loops for variables that change by a predictable amount across iterations:

```c
int p = 0;
#pragma omp simd linear(p:1)
for (int i = 0; i < n; i++) {
    foo(p);
    p++;  // p increases by 1 in each iteration
}
```

## ThreadPrivate Variables

Global variables can be made private to each thread across all parallel regions using the `threadprivate` directive:

```c
int x;
#pragma omp threadprivate(x)

void foo() {
    #pragma omp parallel
    {
        // Each thread has its own persistent copy of x
        x = omp_get_thread_num();
    }
    
    #pragma omp parallel
    {
        // Same value from previous parallel region
        printf("Thread %d has x = %d\n", omp_get_thread_num(), x);
    }
}
```

## Copyin Clause

Used with `threadprivate` variables to initialize thread-private copies from the main thread's value:

```c
int x = 10;
#pragma omp threadprivate(x)

void foo() {
    x = 15;  // Main thread sets x to 15
    
    #pragma omp parallel copyin(x)
    {
        // All threads start with x = 15
    }
}
```

## Copyprivate Clause

Used with `single` to broadcast values from one thread to all others:

```c
#pragma omp parallel
{
    int a;
    
    #pragma omp single copyprivate(a)
    {
        a = compute_value();  // Only one thread computes this
    }
    
    // All threads now have the same value for a
}
```

## Common Pitfalls and Best Practices

### Race Conditions

Race conditions occur when multiple threads update a shared variable without proper synchronization:

```c
// BAD: Race condition
#pragma omp parallel
{
    counter++;  // Multiple threads may update counter simultaneously
}

// GOOD: Using critical section
#pragma omp parallel
{
    #pragma omp critical
    {
        counter++;
    }
}

// BETTER: Using reduction
#pragma omp parallel reduction(+:counter)
{
    counter++;
}
```

### False Sharing

False sharing occurs when variables used by different threads are located on the same cache line, causing cache invalidation and reduced performance:

```c
// Potential false sharing
int counter[NUM_THREADS];

#pragma omp parallel
{
    int tid = omp_get_thread_num();
    counter[tid]++;  // Different threads update adjacent array elements
}

// Better: Pad array to avoid false sharing
struct padded_int {
    int value;
    char padding[CACHE_LINE_SIZE - sizeof(int)];
};
struct padded_int counter[NUM_THREADS];
```

### Privatizing Loop Variables

Loop variables are automatically private in loop constructs but need explicit privatization in other contexts:

```c
// Loop variable i is implicitly private here
#pragma omp parallel for
for (int i = 0; i < n; i++) {
    // Work with i
}

// But outside of a loop construct:
int i;
#pragma omp parallel
{
    // Need to make i private to avoid conflicts
    #pragma omp for private(i)
    for (i = 0; i < n; i++) {
        // Work with i
    }
}
```

### Default(none) for Safety

Using `default(none)` forces explicit declaration of all variables, preventing accidental sharing:

```c
int x, y, i, n;
double *a;

#pragma omp parallel default(none) shared(x, a, n) private(y, i)
{
    // Must declare data-sharing attribute for all variables used
}
```

### Atomic vs Critical

Use `atomic` for simple updates as it's more efficient than `critical`:

```c
// Atomic is more efficient for simple operations
#pragma omp parallel
{
    #pragma omp atomic
    counter++;
}

// Critical is needed for complex operations
#pragma omp parallel
{
    #pragma omp critical
    {
        if (value > max_value) {
            max_value = value;
            max_index = i;
        }
    }
}
```

## Advanced Techniques

### Using Firstprivate with Dynamic Allocation

When working with dynamically allocated memory:

```c
char* buffer = malloc(SIZE);
// Initialize buffer...

#pragma omp parallel firstprivate(buffer)
{
    // Each thread gets its own pointer to the same memory!
    // This is probably not what you want
}

// Solution: Each thread allocates its own memory
#pragma omp parallel
{
    char* private_buffer = malloc(SIZE);
    // Work with private_buffer
    free(private_buffer);
}
```

### Thread Local Storage

For C11/C++11 and later, you can use `_Thread_local` or `thread_local` for persistent thread-specific storage:

```c
// C11
_Thread_local int counter = 0;

// C++11
thread_local int counter = 0;

void increment() {
    counter++;  // Each thread has its own counter
}
```

### Memory Consistency

OpenMP provides a relaxed consistency model. Use flush when you need to ensure memory consistency:

```c
int flag = 0, data = 0;

#pragma omp parallel sections num_threads(2)
{
    #pragma omp section
    {
        data = compute_data();
        #pragma omp flush(data)
        flag = 1;
        #pragma omp flush(flag)
    }
    
    #pragma omp section
    {
        while (1) {
            #pragma omp flush(flag)
            if (flag) {
                #pragma omp flush(data)
                use_data(data);
                break;
            }
        }
    }
}
```

## Example: Vector Addition Using Different Data-Sharing Attributes

```c
#include <stdio.h>
#include <omp.h>

#define N 1000000

void vector_add_shared(float* a, float* b, float* c, int n) {
    int i;
    #pragma omp parallel shared(a,b,c,n)
    {
        #pragma omp for
        for (i = 0; i < n; i++) {
            c[i] = a[i] + b[i];
        }
    }
}

void vector_add_with_reduction(float* a, float* b, float* c, int n, float* sum) {
    float local_sum = 0.0;
    int i;
    
    #pragma omp parallel firstprivate(local_sum) shared(a,b,c,n,sum)
    {
        #pragma omp for reduction(+:local_sum)
        for (i = 0; i < n; i++) {
            c[i] = a[i] + b[i];
            local_sum += c[i];
        }
    }
    
    *sum = local_sum;
}

int main() {
    float* a = (float*)malloc(N * sizeof(float));
    float* b = (float*)malloc(N * sizeof(float));
    float* c = (float*)malloc(N * sizeof(float));
    float sum = 0.0;
    
    // Initialize arrays
    for (int i = 0; i < N; i++) {
        a[i] = i * 0.5;
        b[i] = i * 1.0;
    }
    
    vector_add_with_reduction(a, b, c, N, &sum);
    
    printf("Sum of all elements: %f\n", sum);
    
    free(a);
    free(b);
    free(c);
    
    return 0;
}
```

This example demonstrates various data-sharing attributes in practice, including shared arrays, private loop indices, and reduction variables.

## Summary

Proper data sharing is crucial for correct and efficient OpenMP programs:

1. **Use `shared` for** read-only data or variables that need coordinated updates
2. **Use `private` for** loop indices and temporary variables
3. **Use `firstprivate` when** threads need initialized copies of variables
4. **Use `lastprivate` when** you need the final value of a variable
5. **Use `reduction` for** safely accumulating values across threads
6. **Use `default(none)` to** enforce explicit data sharing and prevent bugs
7. **Be careful with** dynamically allocated memory in parallel regions
8. **Consider using** atomic operations instead of critical sections when possible
9. **Be aware of** false sharing when threads access adjacent memory locations