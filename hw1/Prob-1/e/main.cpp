#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cilk.h>
#include "../include/common.hpp"

int len = -1;
char *X, *Y;
int **G, **D, **I;

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
    int blksize = n / q;

    int nsquares = 1;
    int dirn = +1;
    int lrow = 0; // The index of the leftmost row
    int lcol = 0; // The index of the leftmost column

    do {
        // Process all squares in parallel
        CFOR(int i = 0; i < nsquares; ++i) {
            int r1 = (lrow-i)*blksize;
            int r2 = r1 + blksize;
            int c1 = (lcol+i)*blksize;
            int c2 = c1 + blksize;
            agc_dnc(s1, s2, r1, r2, c1, c2);
        }

        if (nsquares == blksize) {
            dirn = -1;
        }
        nsquares += dirn;
        if (dirn) {
            lrow += 1;
        } else {
            lcol += 1;
        }

    } while (nsquares > 0);
    return G[n][n];
}

int
main() {
    int nworkers = cilk::current_worker_count();
    // printf("Worker Count: %d\n", nworkers);

    /*
      for (int i = 1; i < 67; ++i) {
      fprintf(stderr, "i: %d, rounded: %d\n", i, pow2round(i));
      }
    */

    user_input();
    int answer = agc_cache_efficient(X, Y, len, nworkers * nworkers);
    printf("%d\n", answer);

}
