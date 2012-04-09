#include <iostream>
#include <stdio.h>
#include <vector>
#include <algorithm>

using namespace std;

template <typename T>
struct median_t {
    T* array;
    size_t index;
    size_t rank;

    median_t(T *a = NULL, size_t i = 0, size_t r = 0)
        : array(a), index(i), rank(r)
    { }

    T&
    operator()() {
        return array[index];
    }
};

template <typename T>
median_t<T>
median(T *a1, T *a2, size_t i1, size_t j1, size_t i2, size_t j2) {
    if (i1 == j1) {
        return median_t<T>(a1, i2 + (j2-i2)/2, (j2-i2)/2 + 1);
    }
    if (i2 == j2) {
        return median_t<T>(a1, i1 + (j1-i1)/2, (j1-i1)/2 + 1);
    }
    size_t size = (j1 - i1) + (j2 - i2);
    size_t middle = size/2;

    // Assume that median is in a1
    size_t i = i1, j = j1, m;
    while (i != j) {
        m = i + (j-i)/2;
        // printf("check if %d (index %d) is the median\n", a1[m], m);

        // Is a1[m] the median?
        size_t k = i2 + (middle - (m - i1 + 1));
        // printf("k = %d\n", k);

        if (k > j2) {
            // Out of bounds. Median is in left side.
            j = m;
            continue;
        }

        if (k + 1 < j2) {
            if (a1[m] >= a2[k] && a1[m] <= a2[k+1]) {
                // Found the median.
                return median_t<T>(a1, m, middle);
            }
            if (a1[m] >= a2[k]) {
                // a1[m] is far too big.
                j = m;
                continue;
            }
            // a1[m] is far too small.
            i = m + 1;
        } else {
            if (a1[m] >= a2[k]) {
                // Found the median.
                return median_t<T>(a1, m, middle);
            }
            // a1[m] is far too small.
            i = m + 1;
        }

    } // while ()

    return median(a2, a1, i2, j2, i1, j1);
}

int
main() {
    int a1[] = { 23, 45, 55, 56, 57, 58, 59, 65, 75, 85 };
    int a2[] = { 10, 20, 30, 40, 50, 60, 70 };
    int s1 = sizeof(a1)/sizeof(int), s2 = sizeof(a2)/sizeof(int);

    vector<int> out(s1 + s2);

    vector<int> v(a1, a1 + s1);
    v.insert(v.end(), a2, a2 + s2);
    sort(v.begin(), v.end());

    median_t<int> m = median(a1, a2, 0, s1, 0, s2);
    printf("Median: %s[%d] = %d (rank: %d)\n", (m.array == a1 ? "a1" : "a2"), m.index, m(), m.rank);
    printf("Median by sorting: %d, #elements = %d\n", v[v.size()/2], v.size());

    size_t i = m.index + 1, j = m.rank - (m.index);
    std::merge(a1, a1+i, a2, a2+j, out.begin());
    std::merge(a1+i, a1+s1, a2+j, a2+s2, out.begin() + m.rank + 1);

    for (size_t k = 0; k < out.size(); ++k) {
        printf("%2d ", out[k]);
    }
    printf("\n");

    m = median(a2, a1, 0, s2, 0, sizeof(a1)/sizeof(int));
    printf("Median: %s[%d] = %d (rank: %d)\n", (m.array == a1 ? "a1" : "a2"), m.index, m(), m.rank);
    printf("Median by sorting: %d, #elements = %d\n", v[v.size()/2], v.size());

    v.clear();
    v.assign(a1 + 1, a1 + s1);
    v.insert(v.end(), a2, a2 + s2);
    sort(v.begin(), v.end());

    m = median(a1, a2, 1, sizeof(a1)/sizeof(int), 0, sizeof(a2)/sizeof(int));
    printf("Median: %s[%d] = %d (rank: %d)\n", (m.array == a1 ? "a1" : "a2"), m.index, m(), m.rank);
    printf("Median by sorting: %d, #elements = %d\n", v[v.size()/2], v.size());

    v.clear();
    v.assign(a1, a1 + s1);
    v.insert(v.end(), a2 + 1, a2 + s2);
    sort(v.begin(), v.end());

    m = median(a2, a1, 1, sizeof(a2)/sizeof(int), 0, sizeof(a1)/sizeof(int));
    printf("Median: %s[%d] = %d (rank: %d)\n", (m.array == a1 ? "a1" : "a2"), m.index, m(), m.rank);
    printf("Median by sorting: %d, #elements = %d\n", v[v.size()/2], v.size());

    v.clear();
    v.assign(a1 + 2, a1 + s1);
    v.insert(v.end(), a2, a2 + s2);
    sort(v.begin(), v.end());

    m = median(a1, a2, 2, sizeof(a1)/sizeof(int), 0, sizeof(a2)/sizeof(int));
    printf("Median: %s[%d] = %d (rank: %d)\n", (m.array == a1 ? "a1" : "a2"), m.index, m(), m.rank);
    printf("Median by sorting: %d, #elements = %d\n", v[v.size()/2], v.size());

    v.clear();
    v.assign(a1, a1 + s1);
    v.insert(v.end(), a2 + 2, a2 + s2);
    sort(v.begin(), v.end());

    m = median(a2, a1, 2, sizeof(a2)/sizeof(int), 0, sizeof(a1)/sizeof(int));
    printf("Median: %s[%d] = %d (rank: %d)\n", (m.array == a1 ? "a1" : "a2"), m.index, m(), m.rank);
    printf("Median by sorting: %d, #elements = %d\n", v[v.size()/2], v.size());

    v.clear();
    v.assign(a1 + 3, a1 + s1);
    v.insert(v.end(), a2, a2 + s2);
    sort(v.begin(), v.end());

    m = median(a1, a2, 3, sizeof(a1)/sizeof(int), 0, sizeof(a2)/sizeof(int));
    printf("Median: %s[%d] = %d (rank: %d)\n", (m.array == a1 ? "a1" : "a2"), m.index, m(), m.rank);
    printf("Median by sorting: %d, #elements = %d\n", v[v.size()/2], v.size());

    v.clear();
    v.assign(a1, a1 + s1);
    v.insert(v.end(), a2 + 3, a2 + s2);
    sort(v.begin(), v.end());

    m = median(a2, a1, 3, sizeof(a2)/sizeof(int), 0, sizeof(a1)/sizeof(int));
    printf("Median: %s[%d] = %d (rank: %d)\n", (m.array == a1 ? "a1" : "a2"), m.index, m(), m.rank);
    printf("Median by sorting: %d, #elements = %d\n", v[v.size()/2], v.size());

}
