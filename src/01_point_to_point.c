/*
 * Distributed Systems Lab - Assignment 1
 * Topic: Point-to-Point Data Transmission (System 1 to System 2)
 * Student: Girish Paikra
 * File: 01_point_to_point.c
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE 8
#define MSG_TAG 101

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
            fprintf(stderr, "Error: This program requires exactly 2 processes.\n");
            fprintf(stderr, "Usage: mpirun -np 2 ./01_point_to_point\n");
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0) {
        /* System 1 (Rank 0): Sender */
        double data[ARRAY_SIZE];
        printf("\n======================================================================\n");
        printf("[SYSTEM 1] Hostname: %s | Rank: %d\n", hostname, rank);
        printf("[SYSTEM 1] Role: SENDER\n");
        printf("======================================================================\n");

        printf("[SYSTEM 1] Initializing data array:\n");
        for (int i = 0; i < ARRAY_SIZE; i++) {
            data[i] = 25.4 + (i * 3.75);
            printf("   data[%d] = %6.2f\n", i, data[i]);
        }

        printf("\n[SYSTEM 1] Sending %d doubles to System 2...\n", ARRAY_SIZE);

        /* Send data array to Rank 1 */
        MPI_Send(data, ARRAY_SIZE, MPI_DOUBLE, 1, MSG_TAG, MPI_COMM_WORLD);

        printf("[SYSTEM 1] Transmission complete.\n");
        printf("======================================================================\n\n");

    } else if (rank == 1) {
        /* System 2 (Rank 1): Receiver */
        double received_data[ARRAY_SIZE];
        MPI_Status status;

        /* Receive data array from Rank 0 */
        MPI_Recv(received_data, ARRAY_SIZE, MPI_DOUBLE, 0, MSG_TAG, MPI_COMM_WORLD, &status);

        printf("\n======================================================================\n");
        printf("[SYSTEM 2] Hostname: %s | Rank: %d\n", hostname, rank);
        printf("[SYSTEM 2] Role: RECEIVER\n");
        printf("======================================================================\n");
        printf("[SYSTEM 2] Received %d elements from Source Rank %d (Tag: %d)\n",
               ARRAY_SIZE, status.MPI_SOURCE, status.MPI_TAG);

        printf("[SYSTEM 2] Verifying received values:\n");
        double sum = 0.0;
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("   received_data[%d] = %6.2f\n", i, received_data[i]);
            sum += received_data[i];
        }

        printf("[SYSTEM 2] Data processing complete. Average = %6.2f\n", sum / ARRAY_SIZE);
        printf("======================================================================\n\n");
    }

    MPI_Finalize();
    return 0;
}
