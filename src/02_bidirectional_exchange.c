/*
 * Distributed Systems Lab - Assignment 1
 * Topic: Bidirectional Data Exchange (MPI_Sendrecv)
 * Student: Girish Paikra
 * File: 02_bidirectional_exchange.c
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 64

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
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int partner_rank = (rank == 0) ? 1 : 0;
    char send_msg[BUFFER_SIZE];
    char recv_msg[BUFFER_SIZE];

    /* Prepare messages */
    if (rank == 0) {
        snprintf(send_msg, BUFFER_SIZE, "Token #A9401 from System 1 [%s]", hostname);
    } else {
        snprintf(send_msg, BUFFER_SIZE, "Token #B7208 from System 2 [%s]", hostname);
    }

    int send_tag = rank + 200;
    int recv_tag = partner_rank + 200;

    printf("[SYSTEM %d (%s)] Outgoing message: \"%s\"\n", rank + 1, hostname, send_msg);
    printf("[SYSTEM %d (%s)] Initiating MPI_Sendrecv with System %d...\n",
           rank + 1, hostname, partner_rank + 1);

    MPI_Status status;

    /* MPI_Sendrecv safely sends and receives simultaneously to avoid deadlock */
    MPI_Sendrecv(
        send_msg, BUFFER_SIZE, MPI_CHAR, partner_rank, send_tag,
        recv_msg, BUFFER_SIZE, MPI_CHAR, partner_rank, recv_tag,
        MPI_COMM_WORLD, &status
    );

    printf("\n>>> [SYSTEM %d (%s)] EXCHANGE SUCCESSFUL! <<<\n", rank + 1, hostname);
    printf("    -> Sent to System %d    : \"%s\"\n", partner_rank + 1, send_msg);
    printf("    <- Received from System %d: \"%s\"\n", partner_rank + 1, recv_msg);
    printf("    <- Tag: %d\n\n", status.MPI_TAG);

    MPI_Finalize();
    return 0;
}
