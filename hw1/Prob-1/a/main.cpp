#include <iostream>
#include <cassert>
#include <cstring>
#include <utility>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdio>

#define G_I 2
#define G_E 1
#define S(X,Y) ((X==Y)?0:1)
int n;
char *X, *Y;
int **D, **I, **G;
#define MAT_OP 0
#define DEL_OP 1
#define INS_OP 2

void init() {
    std::cin >> n;
    X = (char *) malloc(sizeof(char)*(n+10));
    Y = (char *) malloc(sizeof(char)*(n+10));
    assert(X && Y);
    D = (int **) calloc((n+10), sizeof(int *));
    I = (int **) calloc((n+10), sizeof(int *));
    G = (int **) calloc((n+10), sizeof(int *));
    
    assert(D && I && G);
    for(int i = 0; i < (n+10)+1; i++) {
        D[i] = (int *) calloc((n+10), sizeof(int));
        I[i] = (int *) calloc((n+10), sizeof(int));
        G[i] = (int *) calloc((n+10), sizeof(int));
        assert(D[i] && I[i] && G[i]);
    }

    std::cin >> X;
    std::cin >> Y;
    
    for(int i = n; i >= 1; i--) {
        X[i] = X[i-1];
        Y[i] = Y[i-1];
    }
    X[0] = '-';
    Y[0] = '-';
    X[n+1] = '\0';
    Y[n+1] = '\0';
    
    for(int i = 0; i <= n; i++) {
        int j = 0;
        if(i == 0 && j == 0)  
            G[i][j] = 0;
        
        if(i > 0 && j == 0)
            G[i][j] = G_I + (G_E * i);
    }

    for(int j = 0; j <= n; j++) {
        int i = 0;
        if(i == 0 && j == 0)  
            G[i][j] = 0;
        
        if(i == 0 && j > 0)
            G[i][j] = G_I + (G_E * j);
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
                if (G[i-1][j] + G_I + G_E == T) {
                    CUR_OP = MAT_OP;
                }
                i--;
                break;

           case INS_OP:
                T = I[i][j];
                CUR_OP = INS_OP;
                if (G[i][j-1] + G_I + G_E == T) {
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
    for(int i = 0; i < s.size(); i++) {
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

int main() {
    freopen("rand-1024-in.txt", "r", stdin);
    init();
    //D[0][1] = G[1][0];
    D[0][1] = 1;
    std::cout << sizeof(D) << std::endl; 
    
    for(int i = 0; i <= n; i++) {
        for(int j = 0; j <= n; j++) {
            std::cout << i << " " << j << std::endl; 
            // Calculate D
            if(i > 0 && j > 0) {
                D[i][j] = D[i-1][j];
                
                if(G[i-1][j] + G_I < D[i][j]) {
                    D[i][j] = G[i-1][j] + G_I;
                }
                D[i][j] += G_E;
            }
            
            if(i == 0 && j > 0) {
                D[i][j] = G[0][j] + G_E;
            }

            // Calculate I
            if(i > 0 && j > 0) {
                I[i][j] = I[i][j-1];
                
                if(G[i][j-1] + G_I < I[i][j]) {
                    I[i][j] = G[i][j-1] + G_I;
                }

                I[i][j] += G_E;
            }   

            if(i > 0 && j == 0) {
                I[i][j] = G[i][0] + G_E;
            }

            // Calculate G
            if(i > 0 && j > 0) {
                G[i][j] = G[i-1][j-1]+S(X[i],Y[j]);
                
                if(D[i][j] < G[i][j]) {
                    G[i][j] = D[i][j];
                }

                if(I[i][j] < G[i][j]) {
                    G[i][j] = I[i][j];
                }
            }
        }      
    }

    std::cout << G[n][n] << std::endl;
    /* reconstruct_path(); */
}
