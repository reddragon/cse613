#include <iostream>
#include <cassert>
#include <cstring>
#include <utility>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdio>

using namespace std;

int n;
char _X[2048], _Y[2048];
char *X, *Y;
int G[2048][2048];

int solve() {
    for (int i = 0; i < n + 1; ++i) {
        G[0][i] = G[i][0] = i;
    }

    for (int i = 1; i < n + 1; ++i) {
        int cost = n + G[i-1][0];
        int rev = n;
        for (int j = 1; j < n + 1; ++j) {
            int s = X[i] == Y[j] ? 0 : 1;
            G[i][j] = std::min(G[i-1][j-1] + s, G[i-1][j] + 1);
            // G[i][j] = std::min(G[i][j], G[i][j-1] + 1);
            --rev;
            if (!(cost < G[i][j]+rev)) {
                cost = G[i][j]+rev;
            }
            G[i][j] = cost - (n-j);
        }
    }
    return G[n][n];
}

int main() {
    cin>>n;
    cin>>_X>>_Y;

    X = _X-1;
    Y = _Y-1;
    fprintf(stderr, "n: %d\n", n);
    fprintf(stderr, "string1: %s\nstring2: %s\n", _X, _Y);

    int answer = solve();
    printf("%d\n", answer);
}
