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
    int ia[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    int sz = sizeof(ia)/sizeof(int);
    std::vector<int> a(ia, ia+sz);
    parallel_prefix(&*a.begin(), a.size(), 1, std::plus<int>());
    cout<<a<<endl;
}
