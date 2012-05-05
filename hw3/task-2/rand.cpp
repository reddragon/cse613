#include<cstdlib>
#include<set>
#include<iostream>

int main(int argc, char **argv) {
    int n = atoi(argv[1]);
    std::set<int> s;
    srand(0);
    std::cout << n << std::endl;
    for (int i = 0; i < n; i++) {
        int x;
        do {
            x = rand() % (1<<20);
        } while (s.find(x) != s.end());
        s.insert(x);
        std::cout << x << std::endl;
    }
}
