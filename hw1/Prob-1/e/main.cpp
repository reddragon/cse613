#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cilk.h>

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
}

int
main() {
    int ret = system("X=`grep '^processor' /proc/cpuinfo | wc -l`; exit $X");
    int nworkers = WEXITSTATUS(ret);
    printf("Number of CPUs: %d\n", nworkers);
    printf("Worker Count: %d\n", cilk::current_worker_count());

    /*
      for (int i = 1; i < 67; ++i) {
      fprintf(stderr, "i: %d, rounded: %d\n", i, pow2round(i));
      }
    */
}
