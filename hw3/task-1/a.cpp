#include <iostream>
#include <ostream>
#include <stdio.h>
#include <cstdlib>
#include <assert.h>
#include <malloc.h>
#include <vector>

using namespace std;

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

template <typename T>
ostream&
operator<<(ostream &out, vector<T> const &data) {
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

int
MAIN() {
    int ia[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    int sz = sizeof(ia)/sizeof(int);
    std::vector<int> a(ia, ia+sz);
    parallel_prefix(&*a.begin(), a.size(), 1, std::plus<int>());
    cout<<a<<endl;
}
