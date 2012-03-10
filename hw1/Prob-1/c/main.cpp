#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include "../include/common.hpp"

using namespace std;

int len = -1;
char *X, *Y;
int **G, **D, **I;

void init(int);

void user_input() {
    scanf("%d\n", &len);
    init(len + 1);

    X = Y = NULL;
    size_t z;

    getline(&X, &z, stdin);
    getline(&Y, &z, stdin);
}

void init(int dim) {
    G = (int**)calloc(dim, sizeof(int*));
    D = (int**)calloc(dim, sizeof(int*));
    I = (int**)calloc(dim, sizeof(int*));

    for (int i = 0; i < dim; ++i) {
        G[i] = (int*)calloc(dim, sizeof(int));
        D[i] = (int*)calloc(dim, sizeof(int));
        I[i] = (int*)calloc(dim, sizeof(int));
    }
    for (int j = 0; j < dim; ++j) {
        G[0][j] = GI + GE*j;
        G[j][0] = GI + GE*j;
        D[0][j] = G[0][j] + GE;
        I[j][0] = G[j][0] + GE;
    }
    G[0][0] = 0;

}


int solve(char *s1, char *s2, int n) {
    return agc_dnc(s1-1, s2-1, 1, n, 1, n);
}


void printG(int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (j == 0) {
                fprintf(stderr, " %2d", G[i][j]);
            } else {
                fprintf(stderr, " | %2d ", G[i][j]);
            }
        }
        fprintf(stderr, "\n");
    }
}


int main() {
    user_input();

    // s1[len] = s2[len] = '\0';
    // fprintf(stderr, "s1: %s, s2: %s\n", s1, s2);
    int answer = solve(X, Y, len);
    printf("%d\n", answer);
    std::pair<std::string, std::string> rc = reconstruct(X-1, Y-1, len);

    printf("%s\n%s\n", rc.first.c_str(), rc.second.c_str());

    // printG(len);
}
