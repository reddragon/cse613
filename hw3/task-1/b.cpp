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
    int ia[] = { 34, 21, 54, 2, 121, 32, 72, 766, 81, 1, 12, 11, 321, 39 };
    int sz = sizeof(ia)/sizeof(int);
    std::vector<int> a(ia, ia+sz);
    // #define PARTITION_FOR for
    int ppos;

    for (int i = 0; i < 20; ++i) {
        ppos = parallel_partition(&*a.begin(), 0, a.size()-1, a[0], std::less<int>());
    // #define PARTITION_FOR FOR
        cout<<"pivot: "<<a[ppos]<<"::"<<a<<endl;
        
        random_shuffle(a.begin(), a.end());
    }

}
