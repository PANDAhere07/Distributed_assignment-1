/*
 * Distributed Systems Lab - Assignment 1
 * Topic: Distributed Vector Dot-Product (Parallel Workload Partitioning)
 * Student: Girish Paikra
 * File: 04_distributed_array_processing.c
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TOTAL_ELEMENTS 2000000  /* 2 Million Elements */
#define HALF_ELEMENTS (TOTAL_ELEMENTS / 2)

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
            fprintf(stderr, "Error: Requires 2 processes (System 1 and System 2).\n");
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    double *vecA = NULL;
    double *vecB = NULL;
    double *subA = (double*)malloc(sizeof(double) * HALF_ELEMENTS);
    double *subB = (double*)malloc(sizeof(double) * HALF_ELEMENTS);

    if (!subA || !subB) {
        fprintf(stderr, "Memory allocation failed on rank %d\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0) {
        printf("\n======================================================================\n");
        printf(" DISTRIBUTED PARALLEL COMPUTATION: VECTOR DOT-PRODUCT                 \n");
        printf("======================================================================\n");
        printf(" Master System : %s (Rank 0)\n", hostname);
        printf(" Vector Size   : %d elements\n", TOTAL_ELEMENTS);
        printf(" Division      : Each system processes %d elements\n", HALF_ELEMENTS);
        printf("----------------------------------------------------------------------\n");

        vecA = (double*)malloc(sizeof(double) * TOTAL_ELEMENTS);
        vecB = (double*)malloc(sizeof(double) * TOTAL_ELEMENTS);

        /* Initialize input vectors */
        for (int i = 0; i < TOTAL_ELEMENTS; i++) {
            vecA[i] = 1.0;
            vecB[i] = 2.0;
        }

        printf("[SYSTEM 1] Scattering data to System 2...\n");
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double t_start = MPI_Wtime();

    /* Distribute half to System 1 and half to System 2 */
    MPI_Scatter(vecA, HALF_ELEMENTS, MPI_DOUBLE,
                subA, HALF_ELEMENTS, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Scatter(vecB, HALF_ELEMENTS, MPI_DOUBLE,
                subB, HALF_ELEMENTS, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    /* Compute local dot-product */
    double partial_dot = 0.0;
    for (int i = 0; i < HALF_ELEMENTS; i++) {
        partial_dot += subA[i] * subB[i];
    }

    printf("[SYSTEM %d (%s)] Local dot product: %.2f\n",
           rank + 1, hostname, partial_dot);

    /* Reduce partial results to master */
    double global_dot = 0.0;
    MPI_Reduce(&partial_dot, &global_dot, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double t_end = MPI_Wtime();

    if (rank == 0) {
        double expected_result = (double)TOTAL_ELEMENTS * (1.0 * 2.0);
        printf("----------------------------------------------------------------------\n");
        printf("[SYSTEM 1] Global Dot Product Result : %.2f\n", global_dot);
        printf("[SYSTEM 1] Analytical Expected Result: %.2f\n", expected_result);
        printf("[SYSTEM 1] Verification Status       : %s\n",
               (fabs(global_dot - expected_result) < 1e-5) ? "PASSED (MATCH)" : "FAILED");
        printf("[SYSTEM 1] Total Runtime             : %.6f seconds\n", (t_end - t_start));
        printf("======================================================================\n\n");

        free(vecA);
        free(vecB);
    }

    free(subA);
    free(subB);

    MPI_Finalize();
    return 0;
}
