# CS149 Lecture 2 Summary: A Modern Multi-Core Processor

This lecture focuses on how hardware architects have evolved the design of
processors to handle the increasing demands for speed by shifting from
single-threaded performance to various forms of parallelism and memory
efficiency.

## 1. Fighting Latency with the Cache Hierarchy

A significant portion of the lecture is dedicated to the memory wall. While a
processor can execute instructions in a few cycles, fetching data from main
memory (DRAM) can take hundreds of cycles (e.g., 250 cycles). To combat this,
modern chips use a hierarchy of caches (L1, L2, and L3).

- **Locality:** Caches rely on Temporal Locality (reusing data recently
  accessed) and Spatial Locality (accessing data near a recent access).
- **Cache lines:** Data is transferred from memory to caches in chunks called
  cache lines (typically 64 bytes), which effectively "pre-fetches" nearby data
  to reduce future misses.
- **Miss types:** The lecture identifies Cold Misses (first-time data access)
  and Capacity Misses (data evicted because the cache was full).

## 2. Three Dimensions of Parallelism

To increase performance without solely relying on clock frequency (which is
limited by the Power Wall), architects use three main strategies:

### Super-scalar Execution

The hardware automatically looks for independent instructions within a single
instruction stream and executes them simultaneously using multiple execution
units. This requires no change to the program but is limited by the inherent
dependencies in sequential code.

### Multi-core Execution

Architects duplicate the entire core, including the control logic, execution
units, and registers. This allows the chip to run completely independent
instruction streams (threads). However, the programmer must explicitly define
this parallelism, such as by using Pthreads or higher-level abstractions like a
"for all" loop to tell the hardware which iterations are independent.

### SIMD (Single Instruction, Multiple Data)

Instead of duplicating the whole core, architects replicate the ALUs
(Arithmetic Logic Units) but share a single control unit.

- **Vector instructions:** A single instruction (e.g., "ADD") is applied to a
  vector of multiple data elements at once.
- **Efficiency:** This is highly efficient because it amortizes the cost of
  fetching and decoding instructions over many pieces of data.

## 3. The Challenge of Divergence

When using SIMD, the lecture explains that efficiency is heavily dependent on
Coherent Execution.

- If a program contains an if/else block where some data elements follow the
  "if" path and others the "else" path, the threads diverge.
- Because there is only one control unit, the hardware must use masking: it
  runs the "if" path while disabling the "else" units, then vice-versa. In the
  worst case, this can drop processor utilization to 1/8th or 1/32nd of its
  peak capacity.

## 4. Hardware Multi-threading (Latency Hiding)

The final key concept is Hardware Multi-threading, which is used to keep the
processor busy during long memory stalls.

- The core maintains state (registers and program counters) for multiple
  threads simultaneously.
- **Switch on stall:** When one thread requests data from memory and stalls,
  the hardware instantly switches to another thread that is ready to work.
- **Trade-off:** This increases throughput (total work done) and processor
  utilization, though it may slightly increase the latency (time to finish) of
  any individual thread because resources are shared.
