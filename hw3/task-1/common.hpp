#if !defined COMMON_HPP
#define COMMON_HPP

#include <ostream>
#include <vector>
#include <math.h>
#include <algorithm>

#if defined __cilkplusplus
#define MAIN cilk_main
#define FOR cilk_for
#define PARTITION_FOR cilk_for
#else
#define MAIN main
#define FOR for
#define PARTITION_FOR for
#define cilk_spawn
#define cilk_sync
#endif

size_t base_size = 4;

#if defined DEBUG
#define dprintf(ARGS...) fprintf(stderr, ARGS)
#else
#define dprintf(ARGS...)
#endif

#if defined PARTE
#define PARTITION_FOR for
#endif


// a: The array to compute PPC on
// size: The number of elements in a
// jmp: MUST always be 1
// func: The summing operator to use. Typically std::plus<T>()
template <typename T, typename F>
void
parallel_prefix(T *a, size_t size, size_t jmp, F func) {
    // fprintf(stderr, "parallel_prefix(%d, %d)\n", size, jmp);
    size_t step = jmp*2;
    if (step-1 >= size) {
        return;
    }
    FOR (size_t i = step-1; i < size; i += step) {
        a[i] = func(a[i], a[i-jmp]);
    }
    parallel_prefix(a, size, jmp*2, func);
    FOR (size_t j = step-1+jmp; j < size; j += step) {
        a[j] = func(a[j], a[j-jmp]);
    }
}

// a: The array to partition
// q: The first valid index in a
// r: The last valid index in a
// x: The pivot element
// cmp: A less-than comparator. Typically it is std::less<T>()
template <typename T, typename CMP>
size_t
parallel_partition(T *a, size_t q, size_t r, T x, CMP cmp) {
    size_t n = r - q + 1;
    if (n == 1) {
        return q;
    }
    std::vector<T> B, lt, gt;
    B.resize(n);
    lt.resize(n);
    gt.resize(n);

    PARTITION_FOR (size_t i = 0; i < n; ++i) {
        B[i] = a[q+i];
        if (cmp(B[i], x)) {
            lt[i] = 1;
        } else {
            gt[i] = 1;
        }
    }
    parallel_prefix(&*lt.begin(), lt.size(), 1, std::plus<T>());
    parallel_prefix(&*gt.begin(), gt.size(), 1, std::plus<T>());
    size_t k = q + lt[n-1];
    PARTITION_FOR (size_t j = 0; j < n; ++j) {
        if (cmp(B[j], x)) {
            a[q+lt[j]-1] = B[j];
        } else {
            a[k+gt[j]-1] = B[j];
        }
    }
    return k;
}

// a: The array to sort
// q: The first valid index in a
// r: The last valid index in a
// cmp: A less-than comparator. Typically it is std::less<T>()
template <typename T, typename CMP>
void
parallel_randomized_looping_quicksort(T *a, size_t q, size_t r, CMP cmp) {
    dprintf("parallel_randomized_looping_quicksort(%d, %d)\n", q, r);
    // std::sort(a + q, a + r + 1, cmp);
    // return;
    size_t n = r - q + 1;
    if (n <= base_size) {
        std::sort(a + q, a + r + 1, cmp);
        return;
    }
    size_t k = 0;
    do {
        dprintf("n: %d, q: %d, r: %d, k: %d, (%d > %d)?\n", n, q, r, k, std::max(r-k, k-q), (size_t)ceil(3*n/4.0));
        size_t x = q + (rand() % n);
        dprintf("x: %llu\n", x);
        k = parallel_partition(a, q, r, a[x], cmp);
    } while (std::max(r-k, k-q) > ceil(3*n/4.0));
    dprintf("selected k: %llu\n", k);

    if (k > q) {
        cilk_spawn parallel_randomized_looping_quicksort(a, q, k-1, cmp);
    }
    parallel_randomized_looping_quicksort(a, k, r, cmp);
    cilk_sync;
}

template <typename T>
std::ostream&
operator<<(std::ostream &out, std::vector<T> const &data) {
    out<<"[ ";
    for (size_t i = 0; i < data.size(); ++i) {
        out<<data[i];
        if (i < data.size() - 1) {
            out<<",";
        }
        out<<" ";
    }
    out<<"]";
    return out;
}


#endif // COMMON_HPP
