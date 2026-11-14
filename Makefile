# ==============================================================================
# Distributed Systems Laboratory - Makefile
# Project: MPI Inter-System Data Exchange across 2 Networked Nodes
# ==============================================================================

CC = mpicc
CFLAGS = -Wall -Wextra -O2
BIN_DIR = bin
SRC_DIR = src

TARGETS = $(BIN_DIR)/01_point_to_point \
          $(BIN_DIR)/02_bidirectional_exchange \
          $(BIN_DIR)/03_ping_pong_benchmark \
          $(BIN_DIR)/04_distributed_array_processing

.PHONY: all clean run-p2p run-bidi run-pingpong run-compute run-cluster-p2p run-cluster-pingpong help

all: $(BIN_DIR) $(TARGETS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/01_point_to_point: $(SRC_DIR)/01_point_to_point.c
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/02_bidirectional_exchange: $(SRC_DIR)/02_bidirectional_exchange.c
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/03_ping_pong_benchmark: $(SRC_DIR)/03_ping_pong_benchmark.c
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/04_distributed_array_processing: $(SRC_DIR)/04_distributed_array_processing.c -lm
	$(CC) $(CFLAGS) $< -o $@ -lm

# Local 2-process test runs
run-p2p: $(BIN_DIR)/01_point_to_point
	mpirun -np 2 ./$(BIN_DIR)/01_point_to_point

run-bidi: $(BIN_DIR)/02_bidirectional_exchange
	mpirun -np 2 ./$(BIN_DIR)/02_bidirectional_exchange

run-pingpong: $(BIN_DIR)/03_ping_pong_benchmark
	mpirun -np 2 ./$(BIN_DIR)/03_ping_pong_benchmark

run-compute: $(BIN_DIR)/04_distributed_array_processing
	mpirun -np 2 ./$(BIN_DIR)/04_distributed_array_processing

# Distributed 2-system execution across network via hostfile
run-cluster-p2p: $(BIN_DIR)/01_point_to_point
	mpirun -np 2 --hostfile hosts.txt ./$(BIN_DIR)/01_point_to_point

run-cluster-pingpong: $(BIN_DIR)/03_ping_pong_benchmark
	mpirun -np 2 --hostfile hosts.txt ./$(BIN_DIR)/03_ping_pong_benchmark

clean:
	rm -rf $(BIN_DIR)

help:
	@echo "Available targets:"
	@echo "  make all                   - Compile all 4 MPI programs"
	@echo "  make run-p2p               - Run Exercise 1 (Point-to-Point)"
	@echo "  make run-bidi              - Run Exercise 2 (Bidirectional Exchange)"
	@echo "  make run-pingpong          - Run Exercise 3 (Ping-Pong Benchmark)"
	@echo "  make run-compute           - Run Exercise 4 (Distributed Computation)"
	@echo "  make run-cluster-p2p       - Run Exercise 1 over 2 physical machines (hosts.txt)"
	@echo "  make clean                 - Remove compiled binaries"
