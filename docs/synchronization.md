# OpenMP Synchronization Guide

Proper synchronization is critical in parallel programming to avoid race conditions and ensure correct program behavior.

## Implicit Synchronization

Several OpenMP constructs have implicit synchronization (barriers) that you might not be aware of:

- End of a `parallel` region
- End of a `for` worksharing construct (unless `nowait` is specified)
- End of a `sections` worksharing construct (unless `nowait` is specified)
- End of a `single` construct (unless `nowait` is specified)
- End of a worksharing construct with a `reduction` clause

## Explicit Synchronization

### Barrier

The simplest synchronization mechanism is the barrier:

```c
#pragma omp parallel
{
    // All threads execute this code
    
    // Do first part of work
    heavy_computation_part1();
    
    #pragma omp barrier
    // All threads wait here until every thread reaches this point
    
    // Now all threads proceed with second part
    heavy_computation_part2();
}
```

### Critical Sections

To protect a code section from concurrent execution:

```c
#pragma omp parallel
{
    // Compute local result
    double local_result = compute_something();
    
    // Update shared result (only one thread at a time)
    #pragma omp critical
    {
        // Only one thread can be here at a time
        shared_result += local_result;
    }
    // Threads exit the critical section one at a time
}
```

You can name critical sections to have multiple independent ones:

```c
#pragma omp critical(update_x)
{
    x += local_x;
}

// Different critical section, can overlap with the above
#pragma omp critical(update_y)
{
    y += local_y;
}
```

### Atomic Operations

For simple updates to shared variables, use atomic:

```c
#pragma omp parallel for
for (int i = 0; i < n; i++) {
    // This update is performed atomically
    #pragma omp atomic
    counter += array[i];
}
```

Atomic supports various operations:

```c
// Read atomically
#pragma omp atomic read
v = x;

// Write atomically
#pragma omp atomic write
x = expr;

// Update atomically
#pragma omp atomic update
x += expr;

// Capture old value atomically
#pragma omp atomic capture
{v = x; x += expr;}
```

### Locks

For fine-grained control:

```c
#include <omp.h>

omp_lock_t my_lock;

// Initialize the lock
omp_init_lock(&my_lock);

#pragma omp parallel
{
    // Compute something
    double result = compute();
    
    // Acquire lock
    omp_set_lock(&my_lock);
    
    // Protected code
    shared_data[index] = result;
    
    // Release lock
    omp_unset_lock(&my_lock);
    
    // More work without lock
    more_computation();
}

// Destroy the lock when done
omp_destroy_lock(&my_lock);
```

For recursive locks (a thread can acquire the same lock multiple times):

```c
omp_nest_lock_t nest_lock;
omp_init_nest_lock(&nest_lock);
// Use with omp_set_nest_lock and omp_unset_nest_lock
omp_destroy_nest_lock(&nest_lock);
```

### Ordered Sections

To enforce a particular order of execution:

```c
#pragma omp parallel for ordered
for (int i = 0; i < n; i++) {
    // This can execute in parallel
    heavy_computation(i);
    
    #pragma omp ordered
    {
        // This executes in order of loop iterations
        printf("Iteration %d: %f\n", i, results[i]);
    }
}
```

## Synchronization-Free Constructs

Some constructs avoid explicit synchronization:

### Master

```c
#pragma omp parallel
{
    // All threads execute this
    
    #pragma omp master
    {
        // Only master thread executes this
        // No implicit barrier, other threads continue immediately
        setup_data();
    }
    
    // Threads may reach here at different times
}
```

### Tasks with Dependences

Use task dependencies to coordinate without explicit synchronization:

```c
#pragma omp parallel
{
    #pragma omp single
    {
        #pragma omp task depend(out: x)
        {
            initialize_x();
        }
        
        #pragma omp task depend(in: x) depend(out: y)
        {
            // Will only execute after x is initialized
            y = process_x(x);
        }
        
        #pragma omp task depend(in: y)
        {
            // Will only execute after y is computed
            use_y(y);
        }
    }
}
```

## Common Pitfalls

### False Sharing

Occurs when threads update variables on the same cache line:

```c
// Potential false sharing
int counter[16]; // threads update different elements but potentially same cache line

// Avoid false sharing
// Pad array to ensure elements fall on different cache lines
struct padded_int {
    int value;
    char padding[64 - sizeof(int)]; // 64 bytes is a common cache line size
};
struct padded_int counters[16];
```

### Deadlocks

Can occur with nested locks or circular dependencies:

```c
// Potential deadlock
#pragma omp parallel sections
{
    #pragma omp section
    {
        omp_set_lock(&lock_a);
        // Do something
        omp_set_lock(&lock_b); // May deadlock if another thread has lock_b and wants lock_a
        // Do something
        omp_unset_lock(&lock_b);
        omp_unset_lock(&lock_a);
    }
    
    #pragma omp section
    {
        omp_set_lock(&lock_b);
        // Do something
        omp_set_lock(&lock_a); // Deadlock!
        // Do something
        omp_unset_lock(&lock_a);
        omp_unset_lock(&lock_b);
    }
}
```

## Best Practices

1. Minimize synchronization for better performance
2. Use the simplest construct that meets your needs (atomic is usually faster than critical)
3. Make critical sections as small as possible
4. Consider using reduction instead of critical/atomic when applicable
5. Beware of implicit barriers in OpenMP constructs
6. Use lock hierarchies to prevent deadlocks
7. Watch for false sharing with adjacent data elements