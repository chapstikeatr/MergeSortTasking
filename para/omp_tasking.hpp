#ifndef OMPTASKING__HPP
#define OMPTASKING__HPP

#include <functional>
#include <omp.h>

namespace tasking {

void taskwait() {
#pragma omp taskwait
}

void taskstart(std::function<void()> f) {
#pragma omp task untied
  f();
}

void doinparallel(std::function<void()> f, int nbthreads = 1) {
#pragma omp parallel num_threads(nbthreads)
  {
#pragma omp single
    {
      f();
    }
  }
}

}; // namespace tasking

#endif
