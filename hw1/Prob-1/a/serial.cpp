#include <iostream>
#include <cassert>
#include <cstring>
#include <utility>
#include <vector>
#include <algorithm>
#include <string>

#define G_I 2
#define G_E 1
#define SZ 2500
#define S(X,Y) ((X==Y)?0:1)
int n;
char X[SZ], Y[SZ];
int D[SZ][SZ], I[SZ][SZ], G[SZ][SZ];
#define MAT_OP 0
#define DEL_OP 1
#define INS_OP 2

int main() {
    std::cin >> n;
    assert(X && Y);
    
    std::cin >> X;
    std::cin >> Y;

    //std::cout << X << std::endl << Y << endl;
   
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

    for(int i = 0; i <= n; i++) {
        for(int j = 0; j <= n; j++) {
            
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

    std::cout << G[n][n] << std::endl;
    
    
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
                    T = G[i][j];
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
        //std::cout << old_i << " " << old_j << " " << CUR_OP << std::endl;
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
        //std::cout << old_i << " " << old_j << " " << CUR_OP << std::endl;
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
            a.push_back(X[ai-1]);
        }

        if(s[i].second == bi)
            b.push_back('-');
        else {
            bi = s[i].second;
            b.push_back(Y[bi-1]);
        }
    }

    std::cout << a << std::endl;
    std::cout << b << std::endl;
}
