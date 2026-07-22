# Parallel Computing - Stanford CS149

My coursework for **Stanford CS149: Parallel Computing**. Each lecture/assignment
gets its own folder with the solution code, measured results, and a writeup.

## Machine

All performance numbers are measured on my own box (the course targets a quad-core
myth machine, so my core-scaling ceilings are ~2× the handout's):

- **CPU:** AMD Ryzen 7 6800HS - 8 cores / 16 threads (SMT2), AVX2 + FMA
- **Env:** Ubuntu 24.04 (WSL2), g++ 13.3 `-O3`, ISPC 1.28.1 (`avx2-i32x8`)

## Contents

| Lecture | Assignment | Highlights |
|---|---|---|
| [Lecture 1 - asst1](lecture-01-asst1/) | Performance Analysis on a Multi-Core CPU | 7.5× threaded Mandelbrot (interleaved decomposition) · SIMD intrinsics w/ 90→83% utilization analysis · 58-65× ISPC+tasks · divergence best/worst-case sqrt (6.6× vs 0.9×) · bandwidth-bound saxpy · 2.5× k-means via profiling + lock-free point partitioning |

*(more lectures coming as the course progresses)*

## Layout of each assignment folder

```
lecture-XX-.../
  writeup.pdf     - full writeup: answers, graphs, analysis
  RESULTS.md      - raw measured numbers
  code/           - submission-named deliverable files
  asst1/          - complete buildable source tree (my solutions applied)
```

## Building (lecture 1)

```bash
# inside WSL/Linux with ISPC on PATH
cd lecture-01-asst1/asst1/prog1_mandelbrot_threads && make && ./mandelbrot -t 8
# prog6 needs a dataset: build & run the included generator first
cd ../prog6_kmeans && g++ -O3 -march=native gen_data.cpp utils.cpp -o gen_data && ./gen_data
```
