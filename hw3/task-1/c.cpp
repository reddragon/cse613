#include <iostream>
#include <ostream>
#include <stdio.h>
#include <cstdlib>
#include <assert.h>
#include <malloc.h>
#include <vector>
#include "common.hpp"
#include "timer.hpp"

using namespace std;

#if defined PARTE
#define PARTITION_FOR for
#endif

int
MAIN() {
#if 0
    int ia[] = { 34, 42, 54, 2, 121, 32, 72, 766, 81, 1, 12, 11, 37, 39 };
    int sz = sizeof(ia)/sizeof(int);
    std::vector<int> a(ia, ia+sz);
    parallel_randomized_looping_quicksort(&*a.begin(), 0, a.size()-1, std::less<int>());
    cout<<a<<endl;
#else
    int n;
    scanf("%d", &n);
    std::vector<int> a(n);
    for (int i = 0; i < n; ++i) {
        scanf("%d", &a[i]);
    }

    Timer t;
    t.start();
    parallel_randomized_looping_quicksort(&*a.begin(), 0, a.size()-1, std::less<int>());
    double total_sec = t.stop();

    for (int i = 0; i < n; ++i) {
        printf("%d\n", a[i]);
    }

    fprintf(stderr, "time(sec): %f\n", total_sec/1000000.0);

#endif
}
