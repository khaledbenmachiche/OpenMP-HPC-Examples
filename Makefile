CC = gcc
CFLAGS = -Wall -Wextra -O3 -fopenmp
INCLUDE = -I./include

EXAMPLES_DIR = examples
BENCHMARKS_DIR = benchmarks
TESTS_DIR = tests
BIN_DIR = bin

all: directories examples benchmarks tests

directories:
	mkdir -p $(BIN_DIR)

examples: directories
	$(CC) $(CFLAGS) $(INCLUDE) $(EXAMPLES_DIR)/hello_world.c -o $(BIN_DIR)/hello_world
	$(CC) $(CFLAGS) $(INCLUDE) $(EXAMPLES_DIR)/scheduling_comparison.c -o $(BIN_DIR)/scheduling_comparison
	$(CC) $(CFLAGS) $(INCLUDE) $(EXAMPLES_DIR)/task_dependencies.c -o $(BIN_DIR)/task_dependencies -lm

benchmarks: directories
	@echo "Building benchmarks..."
	$(CC) $(CFLAGS) $(INCLUDE) $(BENCHMARKS_DIR)/task_benchmark.c -o $(BIN_DIR)/task_benchmark -lm

tests: directories
	@echo "Building tests..."

clean:
	rm -rf $(BIN_DIR)

.PHONY: all directories examples benchmarks tests clean
