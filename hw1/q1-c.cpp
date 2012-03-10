#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

using namespace std;

int dim = 5000;
int **G, **D, **I;
int gi = 2, ge = 1, match = 1;

void init() {
    G = (int**)calloc(dim, sizeof(int*));
    D = (int**)calloc(dim, sizeof(int*));
    I = (int**)calloc(dim, sizeof(int*));

    for (int i = 0; i < dim + 1; ++i) {
        G[i] = (int*)calloc(dim, sizeof(int));
        D[i] = (int*)calloc(dim, sizeof(int));
        I[i] = (int*)calloc(dim, sizeof(int));
    }
    for (int j = 0; j < dim; ++j) {
        G[0][j] = gi + ge*j;
        G[j][0] = gi + ge*j;
        D[0][j] = G[0][j] + ge;
        I[j][0] = G[j][0] + ge;
    }
    G[0][0] = 0;

}

int GETG(char *s1, char *s2, int i, int j) {
    int v = 0;
    if (i == 0 && j == 0) {
        // do nothing - return 0
    } else if (i == 0 && j > 0) {
        v = gi + ge * j;
     } else if (j == 0 && i > 0) {
        v = gi + i * ge;
    } else {
        int a = D[i][j];
        int b = I[i][j];
        int c = G[i-1][j-1] + (s1[i] == s2[j] ? 0 : match);
        v = std::min(a, std::min(c, b));
    }
    // fprintf(stderr, "G[%d][%d] = %d\n", i, j, v);
    return G[i][j] = v;
}

int GETD(char *s1, char *s2, int i, int j) {
    int v = 10000000;
    if (i == 0 && j > 0) {
        v = G[0][j] + ge;
    } else if (j > 0 && i > 0) {
        int a = D[i-1][j];
        int b = G[i-1][j] + gi;
        v = std::min(a, b) + ge;
    } else {
        // twiddle thumbs
    }
    return D[i][j] = v;
}

int GETI(char *s1, char *s2, int i, int j) {
    int v = 10000000;
    if (i > 0 && j == 0) {
        v = G[i][0] + ge;
    } else if (j > 0 && i > 0) {
        int a = I[i][j-1];
        int b = G[i][j-1] + gi;
        v = std::min(a, b) + ge;
    } else {
        // twiddle thumbs
    }
    return I[i][j] = v;
}

int s(char *s1, char *s2, int r1, int r2, int c1, int c2) {
    if (r2 == r1) {
        // Base Case
        GETD(s1, s2, r1, c1);
        GETI(s1, s2, r1, c1);
        return GETG(s1, s2, r1, c1);
    } else {
        int q11_r1 = r1;
        int q11_r2 = (r1+r2-1)/2;
        int q11_c1 = c1;
        int q11_c2 = (c1+c2-1)/2;

        int q12_r1 = r1;
        int q12_r2 = (r1+r2-1)/2;
        int q12_c1 = (c1+c2-1)/2 + 1;
        int q12_c2 = c2;

        int q21_r1 = (r1+r2-1)/2 + 1;
        int q21_r2 = r2;
        int q21_c1 = c1;
        int q21_c2 = (c1+c2-1)/2;

        int q22_r1 = (r1+r2-1)/2 + 1;
        int q22_r2 = r2;
        int q22_c1 = (c1+c2-1)/2 + 1;
        int q22_c2 = c2;

        s(s1, s2, q11_r1, q11_r2, q11_c1, q11_c2);
        s(s1, s2, q12_r1, q12_r2, q12_c1, q12_c2);
        s(s1, s2, q21_r1, q21_r2, q21_c1, q21_c2);
        return s(s1, s2, q22_r1, q22_r2, q22_c1, q22_c2);
    }
}

int solve(char *s1, char *s2, int n) {
    return s(s1-1, s2-1, 1, n, 1, n);
}

std::pair<std::string, std::string>
reconstruct(char *s1, char *s2, int n) {
    std::string r1, r2;

    int i = n, j = n;

    while (i > 0 || j > 0) {
        if (i == 0) {
            // Ran out of sequence-1
            r2 += s2[j];
            r1 += '-';
            --j;
        } else if (j == 0) {
            // Ran out of sequence-2
            r1 += s1[i];
            r2 += '-';
            --i;
        } else {
            int a = D[i][j];
            int b = I[i][j];
            int c = G[i-1][j-1];
            // fprintf(stderr, "a: %d, b: %d, c: %d\n", a, b, c);
            if (c < a && c < b) {
                // assert(s1[i] == s2[j]);
                r1 += s1[i];
                r2 += s2[j];
                --i; --j;
            } else if (b < a && b < c) {
                r1 += s1[i];
                r2 += '-';
                --i;
            } else {
                // (a < b && a < c)
                r1 += '-';
                r2 += s2[j];
                --j;
            }
        }
    }
    return make_pair(std::string(r1.rbegin(), r1.rend()), std::string(r2.rbegin(), r2.rend()));

}

int main() {
    int len;
    scanf("%d\n", &len);

    dim = len + 10;
    init();

    char *s1 = NULL, *s2 = NULL;
    size_t z;
    getline(&s1, &z, stdin);
    getline(&s2, &z, stdin);

    // s1[len] = s2[len] = '\0';
    // fprintf(stderr, "s1: %s, s2: %s\n", s1, s2);
    int answer = solve(s1, s2, len);
    printf("%d\n", answer);
    std::pair<std::string, std::string> rc = reconstruct(s1-1, s2-1, len);

    printf("%s\n%s\n", rc.first.c_str(), rc.second.c_str());

}
