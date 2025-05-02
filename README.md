# OpenMP-HPC-Examples

A comprehensive collection of examples, benchmarks, and utilities for High-Performance Computing using OpenMP.

## Overview

This repository provides practical examples and resources for learning and implementing parallel programming with OpenMP. It includes carefully crafted examples demonstrating various OpenMP features, benchmarks for performance analysis, and utilities to help with parallel algorithm development.

## Project Structure

- `examples/`: Example programs demonstrating various OpenMP features and patterns
  - Basic parallelism
  - Work sharing constructs
  - Synchronization mechanisms
  - SIMD directives
  - Task-based parallelism
- `benchmarks/`: Performance testing code for evaluating parallel efficiency
- `src/`: Core implementation of parallel algorithms and utilities
- `include/`: Header files with common declarations and helper functions
- `bin/`: Compiled executable binaries
- `docs/`: Comprehensive documentation on OpenMP concepts and best practices
- `tests/`: Unit tests for parallel algorithm implementations
- `scripts/`: Utility scripts for building, running, and analyzing results

## Documentation

Detailed documentation is available in the `docs/` directory:

- [Getting Started](docs/getting_started.md): Introduction to OpenMP and setup instructions
- [OpenMP Guide](docs/openmp_guide.md): Comprehensive guide to OpenMP features
- [OpenMP Constructs](docs/openmp_constructs.md): Detailed explanation of directives and clauses
- [Data Sharing](docs/data_sharing.md): Understanding data sharing between threads
- [Synchronization](docs/synchronization.md): Thread synchronization mechanisms
- [Performance Tips](docs/performance_tips.md): Optimization strategies for OpenMP programs

## Getting Started

### Prerequisites

- A C/C++ compiler with OpenMP support (GCC, Clang, Intel, etc.)
- Basic understanding of parallel programming concepts

### Building the Examples

Use the provided Makefile to build all examples and benchmarks:

```bash
make all
```

Or build specific components:

```bash
make examples   # Build only the examples
make benchmarks # Build only the benchmarks
```

### Running Examples

Executables are available in the `bin/` directory after building:

```bash
./bin/hello_world
./bin/scheduling_comparison
./bin/simd_example
```

### Running Benchmarks

```bash
./bin/matrix_multiply
./bin/task_benchmark
```

Or use the provided script:

```bash
./scripts/run_benchmarks.sh
```

## Contributing

Contributions are welcome! Please feel free to submit pull requests.

