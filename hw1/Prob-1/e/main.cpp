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


int solve(char *s1, char *s2, int n, int q) {
    return agc_cache_efficient(s1-1, s2-1, n, q);
}

int
main(int argc, char *argv[]) {
    int nworkers = 
#if defined __cilkplusplus
        cilk::current_worker_count();
#else
    4;
#endif

    if (argc > 1) {
        nworkers = atoi(argv[1]);
    }

    // printf("Worker Count: %d\n", nworkers);

    user_input();
    int answer = solve(X, Y, len, nworkers);
    printf("%d\n", answer);

    std::pair<std::string, std::string> rc = reconstruct(X-1, Y-1, len);
    printf("%s\n%s\n", rc.first.c_str(), rc.second.c_str());
}
