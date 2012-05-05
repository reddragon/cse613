#include<cstdlib>
#include<set>
#include<iostream>
#include<vector>

int main(int argc, char **argv) {
    int n = atoi(argv[1]);
    std::vector<int> v;
    for (int i = 0; i < n; i++) {
        v.push_back(i);
    }
    random_shuffle(v.begin(), v.end());
    std::cout << n << std::endl;
    for (int i = 0; i < n; i++) {
        std::cout << v[i] << std::endl;
    }
    std::cout << std::endl;
}
