# Parallel Computing - Stanford CS149

My coursework for **Stanford CS149: Parallel Computing**. Lecture notes live in
`Lectures/`, assignment solutions live in `Assignments/`.

## Machine

All performance numbers are measured on my own box (the course targets a quad-core
myth machine, so my core-scaling ceilings are ~2x the handout's):

- **CPU:** AMD Ryzen 7 6800HS - 8 cores / 16 threads (SMT2), AVX2 + FMA
- **Env:** Ubuntu 24.04 (WSL2), g++ 13.3 `-O3`, ISPC 1.28.1 (`avx2-i32x8`)

## Contents

| Lecture | Summary | Assignment | Highlights |
|---|---|---|---|
| Lecture 1: Why Parallelism? Why Efficiency? | [Lecture 1 Summary](Lectures/Lecture%201/Lecture%201%20Summary.md) | [Assignment 1](Assignments/Assignment%201/) | 7.5x threaded Mandelbrot (interleaved decomposition) · SIMD intrinsics w/ 90->83% utilization analysis · 58-65x ISPC+tasks · divergence best/worst-case sqrt (6.6x vs 0.9x) · bandwidth-bound saxpy · 2.5x k-means via profiling + lock-free point partitioning |
| Lecture 2: A Modern Multi-Core Processor | [Lecture 2 Summary](Lectures/Lecture%202/Lecture%202%20Summary.md) | - | Cache hierarchy and locality · superscalar vs multi-core vs SIMD · divergence and masking · hardware multi-threading for latency hiding |
| Lecture 3: Multi-threading, Throughput, and ISPC | [Lecture 3 Summary](Lectures/Lecture%203/Lecture%203%20Summary.md) | - | Switch-on-stall latency hiding · latency vs bandwidth · the 1% efficiency memory wall · ISPC gangs, programIndex/programCount, foreach |

*(more lectures and assignments coming as the course progresses)*

## Repo layout

```
Lectures/
  Lecture N/
    Lecture N Summary.md   - my notes on the lecture
Assignments/
  Assignment N/
    writeup.pdf            - full writeup: answers, graphs, analysis
    RESULTS.md             - raw measured numbers
    code/                  - submission-named deliverable files
    asst1/                 - complete buildable source tree (my solutions applied)
```

## Building (Assignment 1)

```bash
# inside WSL/Linux with ISPC on PATH
cd "Assignments/Assignment 1/asst1/prog1_mandelbrot_threads" && make && ./mandelbrot -t 8
# prog6 needs a dataset: build & run the included generator first
cd ../prog6_kmeans && g++ -O3 -march=native gen_data.cpp utils.cpp -o gen_data && ./gen_data
```
