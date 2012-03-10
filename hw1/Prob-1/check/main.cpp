#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>

using namespace std;

#define NOGAP 1
#define INGAP 2

int gi = 2, ge = 1, match = 1;
int state = NOGAP;

int main() {
    int cost;
    scanf("%d\n", &cost);

    char *s1 = NULL, *s2 = NULL;
    size_t z;
    getline(&s1, &z, stdin);
    getline(&s2, &z, stdin);
    int len = std::min(strlen(s1), strlen(s2));
    assert(len > 0);
    --len;
    s1[len] = s1[len] = '\0';

    // fprintf(stderr, "s1: %s\ns2: %s\n", s1, s2);

    int c = 0;
    for (int i = 0; i < len; ++i) {
        if (s1[i] == s2[i]) {
            state = NOGAP;
        } else if (s1[i] != '-' && s2[i] != '-') {
            state = NOGAP;
            c += match;
        } else {
            if (state == NOGAP) {
                c += gi;
                state = INGAP;
            }
            c += ge;
        }
        // fprintf(stderr, "c: %d\n", c);
    }
    printf("Read Cost: %d, Computed Cost: %d\n", cost, c);

}
