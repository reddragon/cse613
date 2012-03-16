#if !defined COMMON_HPP
#define COMMON_HPP

#include <string>
#include <utility>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#if defined __cilkplusplus
#include <cilk.h>
#endif

#define GI     2
#define GE     1
#define S(X,Y) ((X==Y) ? 0 : 1)

// Dimension for base case of recursive solution.
int base_dimension = 1;

#if defined __cilkplusplus
#define main cilk_main
#define CFOR cilk_for
#define CSPAWN cilk_spawn
#define CSYNC  cilk_sync
#else
#define CFOR for
#define CSPAWN
#define CSYNC
#endif


extern int len;
extern char *X, *Y;
extern int **D, **I, **G;

void init_arrays(int);

void user_input() {
    scanf("%d\n", &len);
    init_arrays(len + 1);

    X = Y = NULL;
    size_t z;

    getline(&X, &z, stdin);
    getline(&Y, &z, stdin);
}

void init_arrays(int dim) {
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

int agc_naive(char *s1, char *s2, int r1, int r2, int c1, int c2) {
    // fprintf(stderr, "[1] r1: %d, r2: %d, c1: %d, c2: %d\n", r1, r2, c1, c2);
    for (; r1 < r2 + 1; ++r1) {
        // fprintf(stderr, "R1: %d\n", r1);
        for(int c = c1; c < c2 + 1; ++c) {
            // fprintf(stderr, "(r1, c1) == (%d, %d)\n", r1, c1);
            GETD(s1, s2, r1, c);
            GETI(s1, s2, r1, c);
            GETG(s1, s2, r1, c);
        }
    }
    // fprintf(stderr, "[2] r1: %d, r2: %d, c1: %d, c2: %d\n", r1, r2, c1, c2);
    return G[r2][c2];
}

int agc_dnc(char *s1, char *s2, int r1, int r2, int c1, int c2) {
    if (r2 - r1 + 1 <= base_dimension) {
        return agc_naive(s1, s2, r1, r2, c1, c2);
    }
    /* 
       else if (r2 == r1) {
       // Base Case (TODO: Make this larger)
       GETD(s1, s2, r1, c1);
       GETI(s1, s2, r1, c1);
       return GETG(s1, s2, r1, c1);
       } 
    */
    else {
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
        CSPAWN agc_dnc(s1, s2, q12_r1, q12_r2, q12_c1, q12_c2);
        agc_dnc(s1, s2, q21_r1, q21_r2, q21_c1, q21_c2);
        CSYNC;
        return agc_dnc(s1, s2, q22_r1, q22_r2, q22_c1, q22_c2);
    }
}

int pow2round(int n) {
    int l2 = 1, n1 = n;
    while (n1 > 1) {
        ++l2;
        n1 >>= 1;
    }
    int r = 1 << (l2-1);
    return r;
}

int agc_cache_efficient(char *s1, char *s2, int n, int q) {
    q = pow2round(q);
    int nblk = q;
    int blksize = n / q;
    // fprintf(stderr, "agc_cache_efficient(%d, %d)::nblk: %d\n", n, q, nblk);

    for (int z = 1; z < nblk*2; ++z) {
        // Process all squares in parallel
        int nsquares = (z <= nblk ? z : nblk*2 - z);
        int lrow = 0; // The index of the leftmost row
        int lcol = 0; // The index of the leftmost column

        if (z <= nblk) {
            lrow = z - 1;
            lcol = 0;
        } else {
            lcol = z - nblk;
            lrow = nblk - 1;
        }

        // fprintf(stderr, "lrow: %d, lcol: %d, nsquares: %d, blksize: %d\n", lrow, lcol, nsquares, blksize);
        CFOR(int i = 0; i < nsquares; ++i) {
            int r1 = (lrow-i)*blksize + 1;
            int r2 = r1 + blksize - 1;
            int c1 = (lcol+i)*blksize + 1;
            int c2 = c1 + blksize - 1;
            // fprintf(stderr, "r1, r2, c1, c2: %d, %d, %d, %d\n", r1, r2, c1, c2);
            agc_dnc(s1, s2, r1, r2, c1, c2);
        }
    }
    return G[n][n];
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
                --i;
            } else if (state == 'I') {
                // Prefer inserts
                if (I[i][j] == G[i][j-1] + GI + GE) {
                    state = 'G';
                }
                r1 += s1[i];
                r2 += '-';
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
