#if !defined COMMON_HPP
#define COMMON_HPP

#include <ostream>
#include <vector>

#if defined __cilkplusplus
#define MAIN cilk_main
#define FOR cilk_for
#else
#define MAIN main
#define FOR for
#endif

template <typename T, typename F>
void
parallel_prefix(T *a, int size, int jmp, F func) {
    fprintf(stderr, "parallel_prefix(%d, %d)\n", size, jmp);
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

    FOR (int i = 0; i < n; ++i) {
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
    FOR (int i = 0; i < n; ++i) {
        if (cmp(B[i], x)) {
            a[q+lt[i]-1] = B[i];
        } else {
            a[k+gt[i]] = B[i];
        }
    }
    return k;
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
