#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>

#include "../include/common.hpp"

using namespace std;

int len = -1;
char *X, *Y;
int **G, **D, **I;
extern int base_dimension;

int solve(char *s1, char *s2, int n) {
    return agc_dnc(s1-1, s2-1, 1, n, 1, n);
}

void printMatrix(int n, int **M) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (j == 0) {
                fprintf(stderr, " %2d", M[i][j]);
            } else {
                fprintf(stderr, " | %2d ", M[i][j]);
            }
        }
        fprintf(stderr, "\n");
    }
}


// 1st argument is Base Case dimension (i.e. 4 => 4x4 is base case) (default: 16)
int main(int argc, char *argv[]) {
    user_input();

    base_dimension = 16;
    if (argc > 1) {
        base_dimension = atoi(argv[1]);
    }
    if (base_dimension < 1 || base_dimension > len) {
        base_dimension = 16;
    }

    // s1[len] = s2[len] = '\0';
    // fprintf(stderr, "s1: %s, s2: %s\n", s1, s2);

#if defined CILKVIEWPLOT
    cilk::cilkview cv;
    cv.start();
#endif // CILKVIEWPLOT

    int answer = solve(X, Y, len);

#if defined CILKVIEWPLOT
    cv.stop();
    cv.dump("agc");
#endif // CILKVIEWPLOT

    printf("%d\n", answer);

    std::pair<std::string, std::string> rc = reconstruct(X-1, Y-1, len);
    printf("%s\n%s\n", rc.first.c_str(), rc.second.c_str());

    // printMat(len, G);
}
