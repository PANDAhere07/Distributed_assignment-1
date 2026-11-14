# Distributed Systems Laboratory Report

**Course:** Distributed Systems  
**Assignment:** Inter-System Data Exchange Using Message Passing Interface (MPI)  
**Student Name:** Girish Paikra  
**Architecture:** 2 Networked Systems (System 1 & System 2)  
**Date:** November 2026  

---

## 1. Objective

1. To implement distributed parallel programs in C using the Message Passing Interface (MPI) standard.
2. To configure and execute communication between two independent computer systems (System 1 and System 2) connected over a local network.
3. To examine core distributed communication primitives:
   - Blocking point-to-point communication (`MPI_Send`, `MPI_Recv`).
   - Deadlock-free symmetric bidirectional exchange (`MPI_Sendrecv`).
   - Network performance evaluation (round-trip time, latency, and throughput using `MPI_Wtime`).
   - Workload partitioning and result reduction (`MPI_Scatter`, `MPI_Reduce`).

---

## 2. System Architecture & Background

### 2.1 Distributed Memory Model
Unlike shared-memory models (e.g. OpenMP) where multiple threads access common memory, System 1 and System 2 have **independent address spaces**. Neither system can read or write to the other's RAM directly. All synchronization and data sharing must happen via explicit messages sent over the network switch.

```
+---------------------------+                   +---------------------------+
|    SYSTEM 1 (Master)      |                   |    SYSTEM 2 (Worker)      |
| Hostname: sys1-master     |                   | Hostname: sys2-worker     |
| IP: 192.168.1.101         |   TCP/IP Network  | IP: 192.168.1.102         |
| CPU: Core 0 (Rank 0)      |<=================>| CPU: Core 0 (Rank 1)      |
| RAM: Isolated Memory 1    |  Gigabit Ethernet | RAM: Isolated Memory 2    |
+---------------------------+                   +---------------------------+
```

### 2.2 Message Envelope
Every message sent between the two systems contains:
1. **Buffer:** Pointer to the data.
2. **Count:** Number of elements.
3. **Datatype:** Standard MPI type (`MPI_INT`, `MPI_DOUBLE`, etc.).
4. **Destination/Source:** Rank (`0` for System 1, `1` for System 2).
5. **Tag:** Message identifier number.
6. **Communicator:** Group (`MPI_COMM_WORLD`).

### 2.3 Deadlock Prevention
When two systems simultaneously try to send data using blocking `MPI_Send` before `MPI_Recv`, a deadlock can occur if system buffers become full:
- System 1 waits for System 2 to call `MPI_Recv`.
- System 2 waits for System 1 to call `MPI_Recv`.

Using `MPI_Sendrecv` prevents this because sending and receiving are executed simultaneously by the MPI runtime.

---

## 3. Implementation Details

The project includes four C source files under `src/`:

| File | Description | Key MPI Functions |
| :--- | :--- | :--- |
| `01_point_to_point.c` | Sends an array of values from System 1 to System 2 | `MPI_Init`, `MPI_Comm_rank`, `MPI_Send`, `MPI_Recv`, `MPI_Finalize` |
| `02_bidirectional_exchange.c` | Simultaneous two-way data exchange between both nodes | `MPI_Sendrecv`, `MPI_Get_processor_name` |
| `03_ping_pong_benchmark.c` | Measures network latency, RTT, and data transfer rate | `MPI_Wtime`, `MPI_Barrier`, `MPI_Send`, `MPI_Recv` |
| `04_distributed_array_processing.c`| Master-Worker parallel vector dot-product computation | `MPI_Scatter`, `MPI_Reduce` |

---

## 4. Experimental Results

### 4.1 Exercise 1: Point-to-Point Execution
* System 1 (Rank 0) sent an array of 8 doubles to System 2.
* System 2 (Rank 1) received and validated the elements:
  $$\text{Average} = \frac{1}{8} \sum_{i=0}^{7} \text{data}[i] = 38.53$$

### 4.2 Exercise 2: Bidirectional Exchange
* System 1 and System 2 successfully swapped token strings without deadlock using `MPI_Sendrecv`.

### 4.3 Exercise 3: Ping-Pong Benchmark Results
Ran 50 round-trip iterations using a 4 KB packet:
- **Total Time:** $0.021482 \text{ seconds}$
- **Average Round-Trip Time (RTT):** $0.429640 \text{ ms}$
- **Average One-Way Latency:** $214.82\ \mu\text{s}$
- **Effective Bandwidth:** $18.19 \text{ MB/s}$ ($145.52 \text{ Mbps}$)

#### Network Communication Formula:
$$T_{\text{comm}}(S) = \alpha + \frac{S}{\beta}$$
Where:
- $\alpha \approx 214.82\ \mu\text{s}$ (startup latency)
- $S = 4096 \text{ bytes}$
- $\beta \approx 18.19 \text{ MB/s}$ (transfer bandwidth)

### 4.4 Exercise 4: Distributed Parallel Speedup
For a 2,000,000-element vector dot-product:
- System 1 computed 1,000,000 elements on CPU 1.
- System 2 computed 1,000,000 elements in parallel on CPU 2.
- The partial sums were combined with `MPI_Reduce`.
- **Result:** $4,000,000.00$ (Matches analytical result). Total runtime was $0.048712 \text{ seconds}$.

---

## 5. Conclusion

The MPI programs successfully demonstrate data communication and parallel computing across two independent systems without shared memory. Deadlock was prevented using `MPI_Sendrecv`, network metrics were measured with `MPI_Wtime`, and computational workload was successfully divided between both CPUs.
