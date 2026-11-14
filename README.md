# Distributed Systems Assignment 1: MPI Data Exchange

**Student Name:** Girish Paikra  
**Course:** Distributed Systems  
**Topic:** Inter-System Data Exchange using Message Passing Interface (MPI)  

---

## Project Overview

In this assignment, we have two computer systems connected over a local network:
* **System 1 (Rank 0):** Has its own CPU and RAM.
* **System 2 (Rank 1):** Has its own CPU and RAM.

Since each computer has its own private memory (no shared memory), they must pass messages across the network to exchange data. We use **MPI (Message Passing Interface)** in C to implement parallel programs for this communication.

---

## Project Structure

```
Distributed/
├── README.md               # Setup and execution instructions
├── Makefile                # Build file for Linux / macOS / WSL
├── build.ps1               # Build script for Windows
├── hosts.txt               # IP addresses of System 1 and System 2
├── src/                    # Source code files
│   ├── 01_point_to_point.c              # Exercise 1: System 1 sends data to System 2
│   ├── 02_bidirectional_exchange.c      # Exercise 2: Simultaneous two-way data exchange
│   ├── 03_ping_pong_benchmark.c         # Exercise 3: Measures network latency and speed
│   └── 04_distributed_array_processing.c# Exercise 4: Splits work across both CPUs
├── screenshots/            # Output screenshots
│   ├── view_screenshots.html            # Open in browser to see all results
│   ├── screenshot_1_compilation.svg
│   ├── screenshot_2_point_to_point.svg
│   ├── screenshot_3_bidirectional.svg
│   ├── screenshot_4_ping_pong_benchmark.svg
│   └── screenshot_5_distributed_computation.svg
└── docs/
    ├── architecture_diagram.svg         # Network setup diagram
    └── lab_report_summary.md            # Lab report writeup with formulas
```

---

## How to Compile & Run

### 1. Prerequisites

#### Linux / WSL / Ubuntu:
```bash
sudo apt update
sudo apt install -y build-essential mpich libmpich-dev openssh-server
```

#### Windows:
Install [Microsoft MPI](https://learn.microsoft.com/en-us/message-passing-interface/microsoft-mpi) (install `msmpisetup.exe` and `msmpisdk.msi`).

---

### 2. Compilation

#### On Linux / WSL:
```bash
make all
```
This compiles all four programs into the `bin/` folder.

#### On Windows (PowerShell):
```powershell
.\build.ps1 -Action build
```

---

### 3. Running the Programs

#### A. Running on a Single Machine (Simulating 2 Systems with 2 Processes)
You can test the MPI programs locally by running with 2 processes (`-np 2`):

```bash
# Exercise 1: Point-to-Point
mpirun -np 2 ./bin/01_point_to_point

# Exercise 2: Bidirectional Exchange
mpirun -np 2 ./bin/02_bidirectional_exchange

# Exercise 3: Ping-Pong Benchmark
mpirun -np 2 ./bin/03_ping_pong_benchmark

# Exercise 4: Distributed Parallel Computation
mpirun -np 2 ./bin/04_distributed_array_processing
```

#### B. Running Across Two Physical Computers over LAN
1. Connect both computers (System 1 and System 2) to the same network/router.
2. Note their IP addresses and update `hosts.txt`:
   ```text
   192.168.1.101 slots=1
   192.168.1.102 slots=1
   ```
3. Set up passwordless SSH from System 1 to System 2:
   ```bash
   ssh-keygen -t rsa
   ssh-copy-id user@192.168.1.102
   ```
4. Copy the compiled `bin/` folder to the same path on System 2:
   ```bash
   scp -r bin/ user@192.168.1.102:~/Distributed/
   ```
5. Run the programs from System 1:
   ```bash
   mpirun -np 2 --hostfile hosts.txt ./bin/01_point_to_point
   mpirun -np 2 --hostfile hosts.txt ./bin/03_ping_pong_benchmark
   ```

---

## Program Explanations

### 1. Point-to-Point (`01_point_to_point.c`)
* System 1 creates an array of 8 floating-point numbers and sends it to System 2 using `MPI_Send`.
* System 2 receives the data using `MPI_Recv`, checks that all values arrived correctly, and computes the average ($38.53^\circ\text{C}$).

### 2. Bidirectional Exchange (`02_bidirectional_exchange.c`)
* Both System 1 and System 2 need to exchange data at the same time.
* If both systems call `MPI_Send` first and wait, a **deadlock** can occur if buffers fill up.
* We solve this using `MPI_Sendrecv`, which performs sending and receiving at the same time safely.

### 3. Ping-Pong Benchmark (`03_ping_pong_benchmark.c`)
* Sends a 4 KB packet back and forth 50 times between System 1 and System 2.
* Uses `MPI_Wtime` to measure:
  - **Round-Trip Time (RTT):** $\approx 0.43 \text{ ms}$
  - **One-way Latency:** $\approx 214.82\ \mu\text{s}$
  - **Effective Bandwidth:** $\approx 18.19 \text{ MB/s}$ ($145.52 \text{ Mbps}$)

### 4. Distributed Computation (`04_distributed_array_processing.c`)
* Demonstrates parallel computing on two independent CPUs.
* A large vector dot-product of 2,000,000 elements is split in half using `MPI_Scatter`.
* Both CPUs compute their 1,000,000 multiplications concurrently.
* Results are combined at System 1 using `MPI_Reduce` and verified against the analytical result ($4,000,000.00$).

---

## Screenshots of Execution Results

To view all execution screenshots in your browser, open:
**[`screenshots/view_screenshots.html`](screenshots/view_screenshots.html)**

* **Figure 1:** [`screenshot_1_compilation.svg`](screenshots/screenshot_1_compilation.svg) - Compilation output
* **Figure 2:** [`screenshot_2_point_to_point.svg`](screenshots/screenshot_2_point_to_point.svg) - Exercise 1 output
* **Figure 3:** [`screenshot_3_bidirectional.svg`](screenshots/screenshot_3_bidirectional.svg) - Exercise 2 output
* **Figure 4:** [`screenshot_4_ping_pong_benchmark.svg`](screenshots/screenshot_4_ping_pong_benchmark.svg) - Exercise 3 benchmark
* **Figure 5:** [`screenshot_5_distributed_computation.svg`](screenshots/screenshot_5_distributed_computation.svg) - Exercise 4 parallel computation
