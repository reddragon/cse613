#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>

#include "../include/common.hpp"

#ifdef WITH_PAPI
#include <papi.h>
#define NUM_EVENTS 2
#define ERROR_RETURN(rv) { fprintf(stderr, "Error %d %s:line %d: \n", rv,__FILE__,__LINE__);  exit(rv); }
#endif


using namespace std;

int len = -1;
char *X, *Y;
int **G, **D, **I;
extern int base_dimension;

int solve(char *s1, char *s2, int n) {
    return agc_dnc(s1-1, s2-1, 1, n, 1, n);
}

void printMatrix(int n, int **M) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (j == 0) {
                fprintf(stderr, " %2d", M[i][j]);
            } else {
                fprintf(stderr, " | %2d ", M[i][j]);
            }
        }
        fprintf(stderr, "\n");
    }
}


// 1st argument is Base Case dimension (i.e. 4 => 4x4 is base case) (default: 16)
// 2nd argument is the input file path
int main(int argc, char *argv[]) {
    if (argc > 2) {
        freopen(argv[2], "r", stdin);
    }

    user_input();

    base_dimension = 16;
    if (argc > 1) {
        base_dimension = atoi(argv[1]);
    }
    if (base_dimension < 1 || base_dimension > len) {
        base_dimension = 16;
    }

    // s1[len] = s2[len] = '\0';
    // fprintf(stderr, "s1: %s, s2: %s\n", s1, s2);

#if defined WITH_PAPI
    int EventSet = PAPI_NULL;
    int retval;
	char errstring[PAPI_MAX_STR_LEN];
	long long values[NUM_EVENTS];

    if((retval = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT )
	{
        fprintf(stderr, "Error: %s\n", errstring);
        exit(1);
	}

    /* Creating event set   */
    if ((retval=PAPI_create_eventset(&EventSet)) != PAPI_OK)
        ERROR_RETURN(retval);
  
    /* Add the array of events PAPI_TOT_INS and PAPI_TOT_CYC to the eventset*/
    if ((retval=PAPI_add_event(EventSet, PAPI_L1_DCM)) != PAPI_OK)
        ERROR_RETURN(retval);

    if ( PAPI_add_event( EventSet, PAPI_L2_DCM ) != PAPI_OK)
        printf("Error PAPI_add_event \n" );
  
    /* Start counting */
    if ( (retval=PAPI_start(EventSet)) != PAPI_OK)
        ERROR_RETURN(retval);
#endif


#if defined CILKVIEWPLOT
    cilk::cilkview cv;
    cv.start();
#endif // CILKVIEWPLOT

    int answer = solve(X, Y, len);

#if defined CILKVIEWPLOT
    cv.stop();
    cv.dump("agc");
#endif // CILKVIEWPLOT

#if defined WITH_PAPI
    if ( (retval=PAPI_stop(EventSet,values)) != PAPI_OK)
        ERROR_RETURN(retval);
    
    std::cout << "L1 Misses: " << values[0] << " L2 Misses: " << values[1] << std::endl;
    
    if ( (retval=PAPI_remove_event(EventSet,PAPI_L1_DCM))!=PAPI_OK)
        ERROR_RETURN(retval);
  
    if ( (retval=PAPI_remove_event(EventSet,PAPI_L2_DCM))!=PAPI_OK)
        ERROR_RETURN(retval);

    /* Free all memory and data structures, EventSet must be empty. */
    if ( (retval=PAPI_destroy_eventset(&EventSet)) != PAPI_OK)
        ERROR_RETURN(retval);
  
    /* free the resources used by PAPI */
    PAPI_shutdown();
#endif


    printf("%d\n", answer);

    std::pair<std::string, std::string> rc = reconstruct(X-1, Y-1, len);
    printf("%s\n%s\n", rc.first.c_str(), rc.second.c_str());

    // printMat(len, G);
}
