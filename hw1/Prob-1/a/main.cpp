#include <iostream>
#include <cassert>
#include <cstring>
#include <utility>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdio>

#ifdef WITH_PAPI
#include <papi.h>
#define NUM_EVENTS 2
#define ERROR_RETURN(rv) { fprintf(stderr, "Error %d %s:line %d: \n", rv,__FILE__,__LINE__);  exit(rv); }
#endif

int n;
char *X, *Y;
int **D, **I, **G;

#define MAT_OP 0
#define DEL_OP 1
#define INS_OP 2
#define GI 2
#define GE 1
#define S(x,y) ((x==y) ? 0 : 1)

void init() {
    scanf("%d\n", &n);
    X = (char *) malloc(sizeof(char)*(n*4));
    Y = (char *) malloc(sizeof(char)*(n*4));
    assert(X && Y);
    D = (int **) calloc((n+10), sizeof(int *));
    I = (int **) calloc((n+10), sizeof(int *));
    G = (int **) calloc((n+10), sizeof(int *));
    
    assert(D && I && G);
    for(int i = 0; i < (n+10); i++) {
        D[i] = (int *) calloc((n+10), sizeof(int));
        I[i] = (int *) calloc((n+10), sizeof(int));
        G[i] = (int *) calloc((n+10), sizeof(int));
        assert(D[i] && I[i] && G[i]);
    }
    
    scanf("%s%s", X, Y);
    
    for(int i = 0; i <= n; i++) {
        int j = 0;
        if(i == 0 && j == 0)  
            G[i][j] = 0;
        
        if(i > 0 && j == 0)
            G[i][j] = GI + (GE * i);
    }

    for(int j = 0; j <= n; j++) {
        int i = 0;
        if(i == 0 && j == 0)  
            G[i][j] = 0;
        
        if(i == 0 && j > 0)
            G[i][j] = GI + (GE * j);
    }
}


void reconstruct_path() {
    int i = n, j = n, CUR_OP = MAT_OP, T;
    
    std::vector< std::pair<int, int> > s;
    do
    {
        int old_i = i, old_j = j;
        switch(CUR_OP) {
            case MAT_OP:
                CUR_OP = MAT_OP;
                T = G[i][j];
                if(D[i][j] == T) {
                    T = D[i][j];
                    CUR_OP = DEL_OP;
                } 
                else if(I[i][j] == T) {
                    T = I[i][j];
                    CUR_OP = INS_OP;
                }
            
                if(CUR_OP == MAT_OP) {
                    i--; j--;
                }
                break;

           case DEL_OP:
                T = D[i][j];
                CUR_OP = DEL_OP;
                if (G[i-1][j] + GI + GE == T) {
                    CUR_OP = MAT_OP;
                }
                i--;
                break;

           case INS_OP:
                T = I[i][j];
                CUR_OP = INS_OP;
                if (G[i][j-1] + GI + GE == T) {
                    CUR_OP = MAT_OP;
                }
                j--;
                break;
        }
        s.push_back(std::make_pair(old_i, old_j));
    } while(i != 0 && j != 0);
    
    while((i == 0) ^ (j == 0)) {
        int old_i = i, old_j = j;
        switch(CUR_OP) {
            case MAT_OP:
                i = (i > 0 ) ? i - 1 : i;
                j = (j > 0 ) ? j - 1 : j;
                break;
            case DEL_OP:
               CUR_OP = MAT_OP;
               j = j - 1;
               break;
            case INS_OP:
               CUR_OP = MAT_OP;
               i = i - 1;
               break;
        }
        s.push_back(std::make_pair(old_i, old_j));
    }

    std::reverse(s.begin(), s.end());
    std::string a, b;
    int ai = 0, bi = 0;
    for(size_t i = 0; i < s.size(); i++) {
        if(s[i].first == ai && s[i].second == bi) 
            continue;

        if(s[i].first == ai) 
            a.push_back('-');
        else {
            ai = s[i].first;
            a.push_back(X[ai]);
        }

        if(s[i].second == bi)
            b.push_back('-');
        else {
            bi = s[i].second;
            b.push_back(Y[bi]);
        }
    }

    std::cout << a << std::endl;
    std::cout << b << std::endl; 
}

void clean_up() {
    free(X);
    free(Y);

    for(int i = 0; i < (n+10); i++) {
        free(D[i]);
        free(I[i]);
        free(G[i]);
    }
    free(D);
    free(I);
    free(G);
}

int main() {
    init();
    std::cout << sizeof(D) << std::endl; 
    
    #ifdef WITH_PAPI
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

    for(int i = 0; i <= n; i++) {
        for(int j = 0; j <= n; j++) {
            // Calculate D
            if(i > 0 && j > 0) {
                D[i][j] = D[i-1][j];
                
                if(G[i-1][j] + GI < D[i][j]) {
                    D[i][j] = G[i-1][j] + GI;
                }
                D[i][j] += GE;
            }
            
            if(i == 0 && j > 0) {
                D[i][j] = G[0][j] + GE;
            }

            // Calculate I
            if(i > 0 && j > 0) {
                I[i][j] = I[i][j-1];
                
                if(G[i][j-1] + GI < I[i][j]) {
                    I[i][j] = G[i][j-1] + GI;
                }

                I[i][j] += GE;
            }   

            if(i > 0 && j == 0) {
                I[i][j] = G[i][0] + GE;
            }

            // Calculate G
            if(i > 0 && j > 0) {
                G[i][j] = G[i-1][j-1]+S(X[i-1],Y[j-1]);
                
                if(D[i][j] < G[i][j]) {
                    G[i][j] = D[i][j];
                }

                if(I[i][j] < G[i][j]) {
                    G[i][j] = I[i][j];
                }
            }
        }      
    }

    #ifdef WITH_PAPI
    if ( (retval=PAPI_stop(EventSet,values)) != PAPI_OK)
        ERROR_RETURN(retval);
    
    std::cout << "L1 Misses: " << values[0] << " L2 Misses: " << values[1] << std::endl;
    /*
    double tot_access = 4 * n * n *n;
    double miss_ratio = values[0] / tot_access;
    printf("%d\t%f\n",n,miss_ratio);
    */

    if ( (retval=PAPI_remove_event(EventSet,PAPI_L1_DCM))!=PAPI_OK)
        ERROR_RETURN(retval);
  
    if ( PAPI_add_event( EventSet, PAPI_L2_DCM ) != PAPI_OK)
        printf("Error PAPI_add_event \n" );

    /* Free all memory and data structures, EventSet must be empty. */
    if ( (retval=PAPI_destroy_eventset(&EventSet)) != PAPI_OK)
        ERROR_RETURN(retval);
  
    /* free the resources used by PAPI */
    PAPI_shutdown();
    #endif

    std::cout << G[n][n] << std::endl;
    reconstruct_path(); 
    clean_up();
}
