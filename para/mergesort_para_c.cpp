#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstdlib>

#include "omp_tasking.hpp"

#define DEBUG 0

static constexpr size_t DEFAULT_THRESHOLD = 1000;

void generateMergeSortData(std::vector<int>& arr, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    arr[i] = rand();
  }
}

void checkMergeSortResult(std::vector<int>& arr, size_t n) {
  bool ok = true;
  for (size_t i = 1; i < n; ++i) {
    if (arr[i] < arr[i - 1]) {
      ok = false;
      break;
    }
  }
  if (!ok) {
    std::cerr << "notok" << std::endl;
  }
}

void merge(int* arr, size_t l, size_t mid, size_t r, int* temp) {
#if DEBUG
  std::cout << l << " " << mid << " " << r << std::endl;
#endif

  if (l >= r) return;

  // Copy the whole range [l, r] into temp[l, r].
  for (size_t i = l; i <= r; ++i) {
    temp[i] = arr[i];
  }

  size_t i = l;        // left half: [l, mid]
  size_t j = mid + 1;  // right half: [mid+1, r]
  size_t k = l;        // write back into arr

  while (i <= mid && j <= r) {
    if (temp[i] <= temp[j]) {
      arr[k++] = temp[i++];
    } else {
      arr[k++] = temp[j++];
    }
  }

  while (i <= mid) {
    arr[k++] = temp[i++];
  }

  while (j <= r) {
    arr[k++] = temp[j++];
  }
}

void mergesort_seq(int* arr, size_t l, size_t r, int* temp) {
  if (l >= r) return;

  size_t mid = l + (r - l) / 2;
  mergesort_seq(arr, l, mid, temp);
  mergesort_seq(arr, mid + 1, r, temp);
  merge(arr, l, mid, r, temp);
}

void mergesort_parallel(int* arr, size_t l, size_t r, int* temp, size_t threshold) {
  if (l >= r) return;

  size_t size = r - l + 1;
  if (size <= threshold) {
    mergesort_seq(arr, l, r, temp);
    return;
  }

  size_t mid = l + (r - l) / 2;

  tasking::taskstart([=]() {
    mergesort_parallel(arr, l, mid, temp, threshold);
  });

  tasking::taskstart([=]() {
    mergesort_parallel(arr, mid + 1, r, temp, threshold);
  });

  tasking::taskwait();
  merge(arr, l, mid, r, temp);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <n> [threads] [threshold]" << std::endl;
    return -1;
  }

  size_t n = std::strtoull(argv[1], nullptr, 10);
  int threads = (argc >= 3) ? std::atoi(argv[2]) : omp_get_max_threads();
  size_t threshold = (argc >= 4) ? std::strtoull(argv[3], nullptr, 10) : DEFAULT_THRESHOLD;

  if (n == 0) {
    std::cerr << 0.0 << std::endl;
    return 0;
  }

  std::vector<int> arr(n);
  generateMergeSortData(arr, n);

#if DEBUG
  for (size_t i = 0; i < n; ++i)
    std::cout << arr[i] << " ";
  std::cout << std::endl;
#endif

  std::vector<int> temp(n);

  auto start = std::chrono::system_clock::now();

  tasking::doinparallel([&]() {
    mergesort_parallel(arr.data(), 0, n - 1, temp.data(), threshold);
  }, threads);

  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;

  std::cerr << elapsed_seconds.count() << std::endl;
  checkMergeSortResult(arr, n);

#if DEBUG
  for (size_t i = 0; i < n; ++i)
    std::cout << arr[i] << " ";
  std::cout << std::endl;
#endif

  return 0;
}
