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

#define BASE_SIZE 4

#if defined DEBUG
#define dprintf(ARGS...) fprintf(stderr, ARGS)
#else
#define dprintf(ARGS...)
#endif


template <typename T, typename F>
void
parallel_prefix(T *a, int size, int jmp, F func) {
    // fprintf(stderr, "parallel_prefix(%d, %d)\n", size, jmp);
    int step = jmp*2;
    if (step-1 >= size) {
        return;
    }
    FOR (int i = step-1; i < size; i += step) {
        a[i] = func(a[i], a[i-jmp]);
    }
    parallel_prefix(a, size, jmp*2, func);
    FOR (int i = step-1+jmp; i < size; i += step) {
        a[i] = func(a[i], a[i-jmp]);
    }
}

template <typename T, typename CMP>
int
parallel_partition(T *a, int q, int r, T x, CMP cmp) {
    int n = r - q + 1;
    if (n == 1) {
        return q;
    }
    std::vector<T> B, lt, gt;
    B.resize(n);
    lt.resize(n);
    gt.resize(n);

    PARTITION_FOR (int i = 0; i < n; ++i) {
        B[i] = a[q+i];
        if (cmp(B[i], x)) {
            lt[i] = 1;
        } else {
            gt[i] = 1;
        }
    }
    parallel_prefix(&*lt.begin(), lt.size(), 1, std::plus<int>());
    parallel_prefix(&*gt.begin(), gt.size(), 1, std::plus<int>());
    int k = q + lt[n-1];
    PARTITION_FOR (int i = 0; i < n; ++i) {
        if (cmp(B[i], x)) {
            a[q+lt[i]-1] = B[i];
        } else {
            a[k+gt[i]-1] = B[i];
        }
    }
    return k;
}

template <typename T, typename CMP>
void
parallel_randomized_looping_quicksort(T *a, int q, int r, CMP cmp) {
    dprintf("parallel_randomized_looping_quicksort(%d, %d)\n", q, r);
    // std::sort(a + q, a + r + 1, cmp);
    // return;
    int n = r - q + 1;
    if (n <= BASE_SIZE) {
        std::sort(a + q, a + r + 1, cmp);
        return;
    }
    int k = 0;
    do {
        dprintf("n: %d, q: %d, r: %d, k: %d, (%d > %d)?\n", n, q, r, k, std::max(r-k, k-q), (int)ceil(3*n/4.0));
        int x = q + (rand() % n);
        dprintf("x: %d\n", x);
        k = parallel_partition(a, q, r, a[x], cmp);
    } while (std::max(r-k, k-q) > ceil(3*n/4.0));
    dprintf("selected k: %d\n", k);

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
