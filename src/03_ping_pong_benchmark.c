/*
 * Distributed Systems Lab - Assignment 1
 * Topic: Ping-Pong Benchmark (Network Latency and Bandwidth)
 * Student: Girish Paikra
 * File: 03_ping_pong_benchmark.c
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_ROUNDS 50
#define PACKET_FLOATS 1024  /* 1024 floats = 4096 bytes per packet */

int main(int argc, char** argv) {
    int rank, size;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    int name_len;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(hostname, &name_len);

    if (size != 2) {
        if (rank == 0) {
            fprintf(stderr, "Error: Benchmark requires exactly 2 nodes.\n");
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int partner_rank = (rank == 0) ? 1 : 0;
    float packet_buffer[PACKET_FLOATS];

    for (int i = 0; i < PACKET_FLOATS; i++) {
        packet_buffer[i] = (float)i * 0.5f;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        printf("\n======================================================================\n");
        printf("           DISTRIBUTED NETWORK PERFORMANCE BENCHMARK                   \n");
        printf("======================================================================\n");
        printf(" Node 1 (System 1) : %s (Rank 0)\n", hostname);
        printf(" Node 2 (System 2) : System 2 (Rank 1)\n");
        printf(" Total Rounds      : %d ping-pong iterations\n", NUM_ROUNDS);
        printf(" Packet Size       : %d floats (%zu bytes / %.2f KB)\n",
               PACKET_FLOATS, sizeof(packet_buffer), (double)sizeof(packet_buffer) / 1024.0);
        printf("----------------------------------------------------------------------\n");
    }

    double t_start = MPI_Wtime();

    for (int round = 0; round < NUM_ROUNDS; round++) {
        if (rank == 0) {
            MPI_Send(packet_buffer, PACKET_FLOATS, MPI_FLOAT, 1, round, MPI_COMM_WORLD);
            MPI_Recv(packet_buffer, PACKET_FLOATS, MPI_FLOAT, 1, round, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            MPI_Recv(packet_buffer, PACKET_FLOATS, MPI_FLOAT, 0, round, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(packet_buffer, PACKET_FLOATS, MPI_FLOAT, 0, round, MPI_COMM_WORLD);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double t_end = MPI_Wtime();

    if (rank == 0) {
        double total_time_sec = t_end - t_start;
        double avg_rtt_sec = total_time_sec / NUM_ROUNDS;
        double avg_one_way_latency_us = (avg_rtt_sec / 2.0) * 1e6;

        double total_bytes = (double)NUM_ROUNDS * 2.0 * sizeof(packet_buffer);
        double throughput_mbps = (total_bytes / (1024.0 * 1024.0)) / total_time_sec;

        printf("[BENCHMARK RESULTS]\n");
        printf("  - Total Elapsed Time      : %.6f seconds\n", total_time_sec);
        printf("  - Average Round-Trip Time  : %.6f ms\n", avg_rtt_sec * 1000.0);
        printf("  - Average One-Way Latency  : %.2f us\n", avg_one_way_latency_us);
        printf("  - Effective Bandwidth      : %.2f MB/sec (%.2f Mbps)\n",
               throughput_mbps, throughput_mbps * 8.0);
        printf("======================================================================\n\n");
    }

    MPI_Finalize();
    return 0;
}
