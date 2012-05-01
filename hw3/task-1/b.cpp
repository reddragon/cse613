#include <iostream>
#include <ostream>
#include <stdio.h>
#include <cstdlib>
#include <assert.h>
#include <malloc.h>
#include <vector>
#include "common.hpp"

using namespace std;


int
MAIN() {
    int ia[] = { 34, 2, 54, 2, 12, 32, 72, 766, 81, 1, 12, 11, 32, 39 };
    int sz = sizeof(ia)/sizeof(int);
    std::vector<int> a(ia, ia+sz);
    parallel_partition(&*a.begin(), 0, a.size()-1, a[0], std::less<int>());
    cout<<a<<endl;
}
