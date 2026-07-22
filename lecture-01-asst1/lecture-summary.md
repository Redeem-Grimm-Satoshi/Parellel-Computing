# CS149 Lecture 1 Summary: Why Parallelism? Why Efficiency?

This lecture serves as an introduction to the fundamental challenges of modern
parallel computing, emphasizing that performance is no longer a matter of simply
waiting for faster hardware, but rather a complex coordination of parallelism,
efficiency, and data movement.

## 1. Parallelism != Automatic Speedup

The lecture demonstrates through real-world experiments (adding lists of
numbers) that doubling the number of workers rarely doubles the speed of the
task.

- **Communication costs:** A major bottleneck in parallelism is the time spent
  communicating between processors. For example, moving a single number across
  a room (or a chip) can completely negate the gains of adding more cores.
- **Workload imbalance:** Speed is often limited by the slowest worker. If one
  processor is assigned more work, or more difficult work, than others, the
  rest of the system sits idle (the "tail" problem).
- **The goal:** The ideal speedup is P (the number of processors), but the
  actual speedup is limited by the minimization of communication and
  synchronization.

## 2. The Shift to Efficiency

While the course title is "Parallel Computing," the instructor argues that
efficiency is often more important than parallelism.

- **Specialization:** Because general-purpose cores have hit physical limits,
  modern chips (like the Apple Bionic or Google TPUs) use heterogeneous
  architectures. These include "big" cores for serial tasks and "small" or
  specialized cores for highly parallel tasks like machine learning and image
  processing.
- **Specialized hardware:** To gain efficiency, architects are moving away from
  general-purpose designs toward specialized processing infrastructure for
  specific domains (e.g., neural engines).

## 3. The "Power Wall"

Historically, software ran faster every year because architects increased clock
frequencies. This ended because of the Power Wall.

- **Power consumption:** Power consumption is approximately proportional to the
  square of the frequency (P ~ f^2).
- **The result:** We can no longer make single "math pipes" faster because they
  would melt. Instead, we use the same power budget to build many slower,
  parallel cores.

## 4. Data Movement: The Real Bottleneck

A recurring theme is that "moving things around" is the primary constraint in
modern systems.

- **Memory abstraction:** Logically, memory is just an array of bytes.
  Physically, accessing main memory (DRAM) is extremely slow, often taking
  hundreds of clock cycles.
- **The cache hierarchy:** To hide this latency, processors use caches
  (L1, L2, L3). These rely on two principles:
  - **Temporal locality:** If you used data once, you will likely use it again
    soon.
  - **Spatial locality:** If you used a piece of data, you will likely need the
    data located right next to it. Hardware exploits this by moving data in
    cache lines (typically 64 bytes) rather than individual bytes.

## 5. Key Takeaway for Developers

Modern software performance is a data movement problem. To write fast code, you
must structure your programs to maximize locality and minimize the costs of
synchronization, as these overheads are often much larger than the actual
computation itself.
