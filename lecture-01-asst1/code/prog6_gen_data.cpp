// Standalone generator for a local data.dat (official one lives on Stanford AFS).
// Mirrors initData()/initCentroids() from main.cpp and writes the same binary
// format as utils.cpp::writeData().
#include <cstdlib>
#include <cmath>
#include <random>
#include <string>
using namespace std;

extern void writeData(string filename, double *data, double *clusterCentroids,
                      int *clusterAssignments, int *M_p, int *N_p, int *K_p,
                      double *epsilon_p);

static double randDouble() { return (double)rand() / (double)RAND_MAX; }
static double dist(double *x, double *y, int nDim) {
  double a = 0.0;
  for (int i = 0; i < nDim; i++) a += pow(x[i] - y[i], 2);
  return sqrt(a);
}

int main() {
  srand(7);
  int M = 200000, N = 100, K = 3;
  double epsilon = 0.1;

  double *data = new double[(size_t)M * N];
  double *cent = new double[(size_t)K * N];
  int *assign = new int[M];

  // Data: 10 Gaussian blobs (matches initData's internal K=10).
  const int KC = 10;
  double *centers = new double[KC * N];
  default_random_engine generator;
  normal_distribution<double> nd(0.0, 0.5);
  for (int k = 0; k < KC; k++)
    for (int n = 0; n < N; n++) centers[k * N + n] = randDouble();
  for (int m = 0; m < M; m++) {
    int sp = rand() % KC;
    for (int n = 0; n < N; n++)
      data[(size_t)m * N + n] = centers[sp * N + n] + nd(generator);
  }

  // Centroids: start close together (as in initCentroids).
  for (int n = 0; n < N; n++) cent[n] = randDouble();
  for (int k = 1; k < K; k++)
    for (int n = 0; n < N; n++) cent[k * N + n] = cent[n] + (randDouble() - 0.5) * 0.1;

  // Initial assignments.
  for (int m = 0; m < M; m++) {
    double md = 1e30; int best = -1;
    for (int k = 0; k < K; k++) {
      double d = dist(&data[(size_t)m * N], &cent[k * N], N);
      if (d < md) { md = d; best = k; }
    }
    assign[m] = best;
  }

  writeData("./data.dat", data, cent, assign, &M, &N, &K, &epsilon);
  delete[] data; delete[] cent; delete[] assign; delete[] centers;
  return 0;
}
