#include "common.hpp"
#include <cilk.h>

void
prlqs(long long int *a, size_t q, size_t r) {
    parallel_randomized_looping_quicksort(a, q, r, std::less<int>());
}

extern "C++" void parallel_randomized_looping_quicksort_CPP(long long int *a, size_t q, size_t r) {
    cilk::run(prlqs, a, q, r);
}

