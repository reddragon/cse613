#include <iostream>
#include <ostream>
#include <stdio.h>
#include <cstdlib>
#include <assert.h>
#include <malloc.h>
#include <vector>
#include "common.hpp"

using namespace std;

#if defined PARTE
#define PARTITION_FOR for
#endif

int
MAIN() {
    int ia[] = { 34, 42, 54, 2, 121, 32, 72, 766, 81, 1, 12, 11, 37, 39 };
    int sz = sizeof(ia)/sizeof(int);
    std::vector<int> a(ia, ia+sz);
    parallel_randomized_looping_quicksort(&*a.begin(), 0, a.size()-1, std::less<int>());
    cout<<a<<endl;
}
