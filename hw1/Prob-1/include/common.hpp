#if !defined COMMON_HPP
#define COMMON_HPP

#include <string>
#include <utility>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>


#define GI     2
#define GE     1
#define S(X,Y) ((X==Y) ? 0 : 1)

extern char *X, *Y;
extern int **D, **I, **G;

int GETG(char *s1, char *s2, int i, int j) {
    int v = 0;
    if (i == 0 && j == 0) {
        // do nothing - return 0
    } else if (i == 0 && j > 0) {
        v = GI + GE * j;
     } else if (j == 0 && i > 0) {
        v = GI + i * GE;
    } else {
        int a = D[i][j];
        int b = I[i][j];
        int c = G[i-1][j-1] + S(s1[i], s2[j]);
        v = std::min(a, std::min(c, b));
    }
    // fprintf(stderr, "G[%d][%d] = %d\n", i, j, v);
    return G[i][j] = v;
}

int GETD(char *s1, char *s2, int i, int j) {
    int v = 10000000;
    if (i == 0 && j > 0) {
        v = G[0][j] + GE;
    } else if (j > 0 && i > 0) {
        int a = D[i-1][j];
        int b = G[i-1][j] + GI;
        v = std::min(a, b) + GE;
    } else {
        // twiddle thumbs
    }
    return D[i][j] = v;
}

int GETI(char *s1, char *s2, int i, int j) {
    int v = 10000000;
    if (i > 0 && j == 0) {
        v = G[i][0] + GE;
    } else if (j > 0 && i > 0) {
        int a = I[i][j-1];
        int b = G[i][j-1] + GI;
        v = std::min(a, b) + GE;
    } else {
        // twiddle thumbs
    }
    return I[i][j] = v;
}

int agc_dnc(char *s1, char *s2, int r1, int r2, int c1, int c2) {
    if (r2 == r1) {
        // Base Case (TODO: Make this larger)
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

        agc_dnc(s1, s2, q11_r1, q11_r2, q11_c1, q11_c2);
        agc_dnc(s1, s2, q12_r1, q12_r2, q12_c1, q12_c2);
        agc_dnc(s1, s2, q21_r1, q21_r2, q21_c1, q21_c2);
        return agc_dnc(s1, s2, q22_r1, q22_r2, q22_c1, q22_c2);
    }
}

std::pair<std::string, std::string>
reconstruct(char *s1, char *s2, int n) {
    std::string r1, r2;

    int i = n, j = n;
    int state = 'G';

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
            int c = G[i-1][j-1] + S(s1[i], s2[j]);
            // fprintf(stderr, "a: %d, b: %d, c: %d\n", a, b, c);

            if (state == 'D') {
                // Prefer deletes
                if (D[i][j] == G[i-1][j] + GI + GE) {
                    state = 'G';
                }
                r1 += '-';
                r2 += s2[j];
                // --j;
                --i;
            } else if (state == 'I') {
                // Prefer inserts
                if (I[i][j] == G[i][j-1] + GI + GE) {
                    state = 'G';
                }
                r1 += s1[i];
                r2 += '-';
                // --i;
                --j;
            } else {
                // Prefer matches
                if (G[i][j] == D[i][j]) { // Delete from sequence-1
                    state = 'D';
                } else if (G[i][j] == I[i][j]) { // Insert (delete from sequence-2)
                    state = 'I';
                } else { // Match
                    assert(G[i][j] == c);
                    r1 += s1[i];
                    r2 += s2[j];
                    --i; --j;
                    state = 'G';
                }
            }
        }
    }
    return make_pair(std::string(r1.rbegin(), r1.rend()), std::string(r2.rbegin(), r2.rend()));

}


#endif // COMMON_HPP
