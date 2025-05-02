#!/bin/bash

# Script to run benchmarks with different thread counts
# Usage: ./scripts/run_benchmarks.sh [benchmark_executable] [max_threads]

set -e

BENCHMARK=${1:-"bin/matrix_multiply"}
MAX_THREADS=${2:-$(nproc)}

if [ ! -f "$BENCHMARK" ]; then
    echo "Error: Benchmark executable '$BENCHMARK' not found."
    echo "Run './scripts/build.sh' first to build the benchmarks."
    exit 1
fi

echo "Running benchmark: $BENCHMARK"
echo "Testing thread counts from 1 to $MAX_THREADS..."
echo "-------------------------------------------"

# Run with different thread counts
for threads in $(seq 1 $MAX_THREADS); do
    echo "Running with $threads thread(s):"
    export OMP_NUM_THREADS=$threads
    $BENCHMARK
    echo "-------------------------------------------"
done

echo "Benchmark complete!"
