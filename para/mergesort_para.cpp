#include "omp_tasking.hpp"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <omp.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#define DEBUG 0
const int THRESHOLD = 16384;

void generateMergeSortData(std::vector<int> &arr, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    arr[i] = rand();
  }
}

void checkMergeSortResult(std::vector<int> &arr, size_t n) {
  bool ok = true;
  for (size_t i = 1; i < n; ++i)
    if (arr[i] < arr[i - 1])
      ok = false;
  if (!ok)
    std::cerr << "notok" << std::endl;
}

// void merge(int *arr, size_t l, size_t mid, size_t r, int *temp) {
//
// #if DEBUG
//   std::cout << l << " " << mid << " " << r << std::endl;
// #endif
//   if (l >= r)
//     return;
//
//   for (size_t i = l; i < mid; ++i)
//     temp[i] = arr[i];
//
//   size_t i = l;
//   size_t j = mid;
//   size_t k = l;
//
//   while (i < mid && j <= r) {
//     if (temp[i] <= arr[j])
//       arr[k++] = temp[i++];
//     else
//       arr[k++] = arr[j++];
//   }
//
//   while (i < mid) {
//     arr[k++] = temp[i++];
//   }
// }

void merge(int *arr, size_t l, size_t mid, size_t r, int *temp) {
#if DEBUG
  std::cout << l << " " << mid << " " << r << std::endl;
#endif

  if (l >= r) return;

  // Here, mid is the first index of the right half.
  // Left half  = [l, mid - 1]
  // Right half = [mid, r]

  // Copy the whole range to temp using the same indices.
  for (size_t i = l; i <= r; ++i) {
    temp[i] = arr[i];
  }

  size_t i = l;    // current index in left half
  size_t j = mid;  // current index in right half
  size_t k = l;    // current write index in arr

  while (i < mid && j <= r) {
    if (temp[i] <= temp[j]) {
      arr[k++] = temp[i++];
    } else {
      arr[k++] = temp[j++];
    }
  }

  while (i < mid) {
    arr[k++] = temp[i++];
  }

  while (j <= r) {
    arr[k++] = temp[j++];
  }
}

void mergesort_seq(int *arr, size_t l, size_t r, int *temp) {
  if (l < r) {
    size_t mid = (l + r) / 2;
    mergesort_seq(arr, l, mid, temp);
    mergesort_seq(arr, mid + 1, r, temp);
    merge(arr, l, mid + 1, r, temp);
  }
}

void mergesort_para(int *arr, size_t l, size_t r, int *temp) {
  if (l < r) {

    size_t size = r - l;

    if (size <= THRESHOLD) {
      mergesort_seq(arr, l, r, temp);
      return;
    }

    size_t mid = l + (r - l) / 2;
    tasking::taskstart([=]() { mergesort_para(arr, l, mid, temp); });
    tasking::taskstart([=]() { mergesort_para(arr, mid + 1, r, temp); });
    tasking::taskwait();

    merge(arr, l, mid + 1, r, temp);
  }
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <n> <nb-threads>" << std::endl;
    return -1;
  }

  // command line parameter
  size_t n = atol(argv[1]);
  int threads = atol(argv[2]);

  // get arr data
  std::vector<int> arr(n);
  generateMergeSortData(arr, n);

#if DEBUG
  for (size_t i = 0; i < n; ++i)
    std::cout << arr[i] << " ";
  std::cout << std::endl;
#endif

  std::vector<int> temp(n);
  // begin timing
  std::chrono::time_point<std::chrono::system_clock> start =
      std::chrono::system_clock::now();

  std::cout << "threads: " << threads << '\n';
  // sort
  tasking::doinparallel(
      [&]() { mergesort_para(&(arr[0]), 0, n - 1, &(temp[0])); }, threads);

  // end timing
  std::chrono::time_point<std::chrono::system_clock> end =
      std::chrono::system_clock::now();
  std::chrono::duration<double> elpased_seconds = end - start;

  // display time to cerr
  std::cerr << elpased_seconds.count() << std::endl;
  checkMergeSortResult(arr, n);

#if DEBUG
  for (size_t i = 0; i < n; ++i)
    std::cout << arr[i] << " ";
  std::cout << std::endl;
#endif

  return 0;
}
