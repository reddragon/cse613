#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../include/common.hpp"

#if defined __cilkplusplus
#include <cilk.h>
#endif

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

int solve(char *s1, char *s2, int n, int q) {
    return agc_cache_efficient(s1-1, s2-1, n, q);
}

int
main() {
    int nworkers = 
#if defined __cilkplusplus
        cilk::current_worker_count();
#else
    4;
#endif

    // printf("Worker Count: %d\n", nworkers);

    /*
      for (int i = 1; i < 67; ++i) {
      fprintf(stderr, "i: %d, rounded: %d\n", i, pow2round(i));
      }
    */

    user_input();
    int answer = solve(X, Y, len, nworkers * nworkers);
    printf("%d\n", answer);

    std::pair<std::string, std::string> rc = reconstruct(X-1, Y-1, len);
    printf("%s\n%s\n", rc.first.c_str(), rc.second.c_str());
}
