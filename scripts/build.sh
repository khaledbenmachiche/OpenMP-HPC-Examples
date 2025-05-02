#!/bin/bash

# Build script for OpenMP project
# Usage: ./scripts/build.sh [debug|release]

set -e

# Default build type
BUILD_TYPE=${1:-release}
BIN_DIR="bin"
mkdir -p $BIN_DIR

# Compiler options
CC=gcc
COMMON_FLAGS="-Wall -Wextra -fopenmp -I./include"

if [ "$BUILD_TYPE" == "debug" ]; then
    echo "Building in DEBUG mode"
    CFLAGS="$COMMON_FLAGS -O0 -g -DDEBUG"
else
    echo "Building in RELEASE mode"
    CFLAGS="$COMMON_FLAGS -O3 -march=native"
fi

# Build examples
echo "Building examples..."
for src in examples/*.c; do
    if [ -f "$src" ]; then
        exe="$BIN_DIR/$(basename ${src%.c})"
        echo "  $src -> $exe"
        $CC $CFLAGS $src -o $exe
    fi
done

# Build benchmarks
echo "Building benchmarks..."
for src in benchmarks/*.c; do
    if [ -f "$src" ]; then
        exe="$BIN_DIR/$(basename ${src%.c})"
        echo "  $src -> $exe"
        $CC $CFLAGS $src -o $exe
    fi
done

# Build tests if requested
if [ "$2" == "tests" ]; then
    echo "Building tests..."
    for src in tests/*.c; do
        if [ -f "$src" ]; then
            exe="$BIN_DIR/$(basename ${src%.c})"
            echo "  $src -> $exe"
            $CC $CFLAGS $src -o $exe
        fi
    done
fi

echo "Build complete!"
