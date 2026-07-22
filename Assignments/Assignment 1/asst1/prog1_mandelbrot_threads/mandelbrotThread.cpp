#include <stdio.h>
#include <stdlib.h>
#include <thread>

#include "CycleTimer.h"

typedef struct {
    float x0, x1;
    float y0, y1;
    unsigned int width;
    unsigned int height;
    int maxIterations;
    int* output;
    int threadId;
    int numThreads;
} WorkerArgs;


extern void mandelbrotSerial(
    float x0, float y0, float x1, float y1,
    int width, int height,
    int startRow, int numRows,
    int maxIterations,
    int output[]);


//
// workerThreadStart --
//
// Thread entrypoint.
void workerThreadStart(WorkerArgs * const args) {

    // INTERLEAVED (strided/cyclic) row decomposition for load balance.
    //
    // A contiguous-block split is badly imbalanced for the Mandelbrot set:
    // rows near the middle of the image are inside/near the set and cost
    // ~maxIterations per pixel, while rows at the top/bottom escape almost
    // immediately. Handing each thread a contiguous band therefore gives
    // some threads ~16x more work than others, and total time is pinned by
    // the slowest thread.
    //
    // Interleaving fixes this: thread t computes rows t, t+numThreads,
    // t+2*numThreads, ... Because expensive and cheap rows are spread
    // cyclically across all threads, every thread receives a near-identical
    // mix of work, so per-thread times converge and speedup approaches the
    // core count. Each mandelbrotSerial() call handles one row (numRows=1);
    // the per-call overhead is negligible relative to the pixel work.
    for (unsigned int row = args->threadId; row < args->height; row += args->numThreads) {
        mandelbrotSerial(args->x0, args->y0, args->x1, args->y1,
                         args->width, args->height,
                         row, 1,
                         args->maxIterations, args->output);
    }
}

//
// MandelbrotThread --
//
// Multi-threaded implementation of mandelbrot set image generation.
// Threads of execution are created by spawning std::threads.
void mandelbrotThread(
    int numThreads,
    float x0, float y0, float x1, float y1,
    int width, int height,
    int maxIterations, int output[])
{
    static constexpr int MAX_THREADS = 32;

    if (numThreads > MAX_THREADS)
    {
        fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
        exit(1);
    }

    // Creates thread objects that do not yet represent a thread.
    std::thread workers[MAX_THREADS];
    WorkerArgs args[MAX_THREADS];

    for (int i=0; i<numThreads; i++) {
      
        // TODO FOR CS149 STUDENTS: You may or may not wish to modify
        // the per-thread arguments here.  The code below copies the
        // same arguments for each thread
        args[i].x0 = x0;
        args[i].y0 = y0;
        args[i].x1 = x1;
        args[i].y1 = y1;
        args[i].width = width;
        args[i].height = height;
        args[i].maxIterations = maxIterations;
        args[i].numThreads = numThreads;
        args[i].output = output;
      
        args[i].threadId = i;
    }

    // Spawn the worker threads.  Note that only numThreads-1 std::threads
    // are created and the main application thread is used as a worker
    // as well.
    for (int i=1; i<numThreads; i++) {
        workers[i] = std::thread(workerThreadStart, &args[i]);
    }
    
    workerThreadStart(&args[0]);

    // join worker threads
    for (int i=1; i<numThreads; i++) {
        workers[i].join();
    }
}

