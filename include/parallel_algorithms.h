#ifndef PARALLEL_ALGORITHMS_H
#define PARALLEL_ALGORITHMS_H

/**
 * Parallel implementation of array reduction
 * @param arr Input array
 * @param size Array size
 * @param initial Initial value for reduction
 * @param op Operation: 0=sum, 1=product, 2=max, 3=min
 * @return Result of reduction operation
 */
double parallel_reduce(const double* arr, int size, double initial, int op);

/**
 * Parallel implementation of array transformation (map)
 * @param in Input array
 * @param out Output array
 * @param size Array size
 * @param func Function pointer: double (*func)(double)
 */
void parallel_transform(const double* in, double* out, int size, double (*func)(double));

/**
 * Parallel implementation of array sorting (mergesort)
 * @param arr Array to sort
 * @param size Array size
 */
void parallel_sort(double* arr, int size);

#endif // PARALLEL_ALGORITHMS_H
