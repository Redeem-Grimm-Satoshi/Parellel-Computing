# CS149 Lecture 3 Summary: Multi-threading, Throughput, and ISPC

Lecture 3 focuses on maximizing processor efficiency through hardware
multi-threading, the fundamental principles of throughput versus latency, and
the software abstractions provided by the Intel SPMD Program Compiler (ISPC).

## 1. Hardware Multi-threading and Latency Hiding

Hardware multi-threading is built on the principle of "switch on stall": if a
thread is waiting for a long-latency event, most commonly a memory request that
can take hundreds of cycles, the hardware should immediately switch to another
task to keep execution units busy.

- **The office hours analogy:** The instructor compares this to a professor
  helping 15 students at office hours. If the professor tells a student to "go
  think about it" and then sits idle, they are inefficient; if they immediately
  move to the next student, they achieve 100% utilization.
- **Costs and trade-offs:** While multi-threading increases overall throughput,
  it does not reduce memory latency; it only hides its effects. Additionally,
  it increases the total time (latency) for any single thread to finish because
  it must share resources with others.
- **Thread requirements:** The number of threads needed for full efficiency is
  determined by the ratio of math operations to memory latency. For a program
  doing 3 cycles of work with a 12-cycle stall, a total of 5 threads are
  required to achieve 100% utilization.

## 2. Throughput: Latency vs. Bandwidth

The lecture uses several analogies to decouple the concepts of how long a task
takes (latency) versus the rate at which tasks are completed
(bandwidth/throughput).

- **Highway 101 analogy:** Driving from San Francisco to Stanford has a fixed
  latency (e.g., 30 minutes). Throughput can be increased by driving faster
  (which also reduces latency), widening the road (adding lanes/cores), or
  using the road more efficiently (reducing the space between cars, similar to
  pipelining).
- **The laundry analogy:** Doing laundry consists of washing, drying, and
  folding. The throughput of the entire pipeline is limited by the slowest
  stage (the dryer), regardless of how fast the other stages are.

## 3. The "Memory Wall" and Efficiency

A major takeaway is that modern processors are often bottlenecked by memory
bandwidth rather than math speed.

- **The 1% efficiency problem:** An NVIDIA GPU can perform roughly 8 trillion
  math operations per second, requiring approximately 100 terabytes of data per
  second to stay busy. Because modern memory systems only provide about 1
  terabyte of bandwidth, simple programs like array additions often run at only
  1% efficiency.
- **Data locality:** Caches only help if data is reused; if a program accesses
  every piece of data only once, it will be entirely limited by the "data
  pipe".

## 4. ISPC Programming Abstractions

The ISPC language is introduced to map these hardware concepts to a Single
Program Multiple Data (SPMD) model.

- **Program instances:** When an ISPC function is called, it spawns a "gang" of
  program instances. Developers use `programCount` (the total number of
  instances) and `programIndex` (the ID of the current instance) to partition
  work.
- **Data partitioning:** Work can be manually assigned by interleaving
  iterations (where each instance takes every Nth element) or blocking them
  (where each instance takes a contiguous chunk).
- **The `foreach` construct:** This keyword allows the compiler to
  automatically decide the most efficient way to map loop iterations across
  the parallel instances.
