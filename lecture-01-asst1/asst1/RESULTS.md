# CS149 asst1 - measured results

Machine: AMD Ryzen 7 6800HS, 8 physical cores / 16 threads (SMT2), AVX2+FMA (no AVX-512).
ISPC 1.28.1, g++ 13.3, Ubuntu 24.04 (WSL2).
NOTE: assignment targets a quad-core (4c/8t) Intel i7; our machine is 8c/16t, so
core-scaling ceilings are ~2x the handout's expectations.

========================================================================
## Program 1 - threads (Mandelbrot), View 1 unless noted
Serial: ~386 ms (view 1).

### BLOCK decomposition (naive, contiguous bands) - View 1 speedup
threads=2 : 1.99x
threads=3 : 1.78x   <-- WORSE than 2! middle heavy band on one thread
threads=4 : 2.33x
threads=5 : 2.51x
threads=6 : 3.04x
threads=7 : 3.43x
threads=8 : 4.03x
threads=16: 7.44x

### BLOCK per-thread times, 8 threads, View 1 (shows imbalance)
thread 0 rows    0..149 :  5.9 ms   (edge, cheap)
thread 7 rows 1050..1199:  7.0 ms   (edge, cheap)
thread 1 rows  150..299 : 32   ms
thread 6 rows  900..1049: 32   ms
thread 2 rows  300..449 : 63   ms
thread 5 rows  750..899 : 65   ms
thread 3 rows  450..599 : 96   ms   (center, expensive)
thread 4 rows  600..749 : 96   ms   (center, expensive)
=> ~16x spread; total time pinned by slowest (~96 ms) => 4x not 8x.

### INTERLEAVED decomposition (cyclic rows) - final answer
View 1:  2->2.01x 3->2.98x 4->3.88x 5->4.88x 6->5.09x 7->6.70x 8->7.54x 16->10.22x
View 2:  2->1.99x 4->3.96x 8->7.27x 16->9.48x
=> ~7.5x @ 8 threads. 16 threads only 10x (SMT: 8 physical cores, HT shares ALUs).

========================================================================
## Program 2 - CS149 fake SIMD intrinsics
clampedExpVector + arraySumVector both PASS. Ragged N (17, 103) pass (tail handled).

Vector utilization vs VECTOR_WIDTH (clampedExp, -s 10000):
WIDTH= 2 : 90.1%   (203143 vec instrs)
WIDTH= 4 : 85.8%   (118213)
WIDTH= 8 : 83.7%   (64533)
WIDTH=16 : 82.6%   (33708)
=> Utilization falls as width grows: divergence from differing exponents means
   each block runs to the MAX exponent in the block; wider blocks more likely
   to contain a large-exponent lane, so more lanes sit masked-off.

========================================================================
## Program 3 - ISPC Mandelbrot (height=800). Theoretical max on this box:
## 8-wide AVX2 float SIMD x 8 cores = 64x (handout quad-core = 32x).

### Part 1 (SIMD only, no tasks)
View 1: 6.05x  |  View 2: 5.14x   (not 8x: SIMD lane divergence - pixels in a
gang have different iteration counts, gang runs to the max iteration in the
group, so lanes that escaped early are wasted. Worst near the set boundary /
black interior where neighbors diverge; View 2 more divergent -> lower speedup.)

### Part 2 (tasks) - tuned mandelbrot_ispc_withtasks
Baseline (2 tasks):  View1 11.59x  View2 8.27x   (only ~2 cores busy, imbalanced halves)
Granularity sweep (speedup over serial, task ISPC):
 ROWS_PER_TASK=1  (800 tasks): v1 22.3  v2 14.0   (per-task launch overhead dominates)
 ROWS_PER_TASK=2  (400):       v1 54.7  v2 27.1
 ROWS_PER_TASK=3  (267):       v1 54.2  v2 46.2
 ROWS_PER_TASK=4  (200):       v1 59.9  v2 45.1
 ROWS_PER_TASK=6  (134):       v1 64.6  v2 51.1   <== CHOSEN (best balance)
 ROWS_PER_TASK=8  (100):       v1 59.7  v2 48.3
 ROWS_PER_TASK=16 (50):        v1 63.2  v2 44.1
 ROWS_PER_TASK=25 (32):        v1 65.1  v2 37.5
FINAL (ROWS_PER_TASK=6): View1 58.6x, View2 47.9x task ISPC (SIMD-only 6.05/5.14). PASS.
Why it works: ~134 small tasks >> 8 cores gives the ISPC runtime enough units to
hand idle cores the next strip on demand (dynamic load balance), while staying
coarse enough to amortize launch overhead (R=1 shows the overhead cliff).

========================================================================
## Program 4 - iterative sqrt (Newton), gang width 8 (avx2-i32x8), N=20M
Serial ~880 ms. Note x=3.0 never converges (guess->0); 2.998 = most iterations.

                         ISPC(SIMD only)   task ISPC(SIMD x cores)
Baseline (random)        5.01x             61.08x
BEST  (all = 2.998)      6.60x             69.52x
WORST (1 hard / gang 8)  0.90x              8.55x

SIMD vs multicore (baseline): SIMD alone ~5x; tasks add ~12x on top (61/5) via
8 cores + SMT with 64 tasks. Combined ~61x.

BEST input (max SIMD): every element identical AND hardest-to-converge (2.998),
so all 8 lanes iterate the same number of times -> zero divergence, full lane
use, and high compute:memory ratio -> 6.6x (near the 8x ceiling; capped by
20M-float memory traffic + fixed per-elem overhead).

WORST input (min SIMD): one 2.998 per gang, other 7 = 1.0 (converge in 0 iters).
Gang still loops to the hard lane count with 7/8 lanes masked-off -> vector does
the same work as scalar for the 1 useful lane -> 0.90x (slightly <1x: masking/
vector overhead on top of no benefit). Textbook demonstration that divergence
destroys SIMD efficiency.

========================================================================
## Program 5 - saxpy (result = scale*X + Y), N=20M, memory-bandwidth bound
[saxpy ispc]      : 10.85 ms  27.47 GB/s
[saxpy task ispc] :  8.01 ms  37.22 GB/s
Speedup from tasks: 1.36x  (NOT ~8x)

Why so little from more cores: saxpy does 2 loads + 1 store and ~1 FMA per
element (arithmetic intensity ~0.08 flop/byte). It is limited by DRAM bandwidth,
not compute. One or two cores already push close to the memory-controller limit,
so extra cores/threads add almost nothing. No substantial speedup is possible on
this platform without reducing memory traffic.

EC1 (why 4*N*sizeof(float), not 3): write-allocate. Storing to result[] first
triggers a read-for-ownership of the destination cache line (it must be fetched
before being partially/fully overwritten), so the "1 store" actually costs
1 read + 1 write. Traffic = load X + load Y + RFO result + write result = 4*N.
Streaming/non-temporal stores (e.g. _mm256_stream_ps) skip the RFO, cutting to
3*N and giving ~4/3 = 1.33x best-case bandwidth improvement.

========================================================================
## Program 6 - K-means (local data.dat: M=200000, N=100, K=3, eps=0.1, 51 iters)

Profiling the baseline (CycleTimer around each phase, summed over all iters):
  computeAssignments = 2529 ms  (68%)   <== hotspot (only phase with O(K*M*N))
  computeCost        =  721 ms  (19%)
  computeCentroids   =  455 ms  (12%)
  Total              = 3705 ms

Optimization: parallelized ONLY computeAssignments across 8 std::threads,
partitioned over data points (each thread owns a disjoint m-range -> no locks).
Also swapped k/m loop nesting to m-outer so each point keeps its running min in
a register (drops the M-sized minDist scratch array).

After:
  computeAssignments = 384 ms   (6.6x on that phase)
  Total = 1473 ms (best of 3)   => OVERALL SPEEDUP 2.52x  (target >= 2.1x) PASS
Correctness: end.log byte-identical to baseline (deterministic result unchanged).

Amdahl note: after the fix, computeCost+computeCentroids (now ~77% of the 1.5s)
dominate; parallelizing computeAssignments alone caps overall at ~1/(0.32)=3.1x.
