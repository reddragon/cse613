#include <iostream>
#include <stdio.h>
#include <map>
#include <vector>
#include <queue>
#include <assert.h>

using namespace std;

typedef map<int, int> matrix_1d_t;
typedef vector<matrix_1d_t> matrix_2d_t;

struct graph_t {
    matrix_2d_t impl;

    graph_t(int nvertices = 0)
        : impl(nvertices + 1)
    { }

    void
    resize(int nvertices) {
        this->impl.resize(nvertices + 1);
    }

    void
    add_edge(int u, int v) {
        assert(u < this->impl.size());
        assert(v < this->impl.size());

        this->impl[u][v] = 1;
    }
};

graph_t graph;
vector<int> d, sources;
int n, m, r;
int infinity = -1;

void
read_input() {
    scanf("%d %d %d", &n, &m, &r);
    graph.resize(n);
    infinity = n + 10;

    int M = m;
    while (M--) {
        int u, v;
        scanf("%d %d", &u, &v);
        graph.add_edge(u, v);
    }

    int R = r;
    while (R--) {
        int s;
        scanf("%d", &s);
        sources.push_back(s);
    }
}

void
serial_bfs(int s) {
    d.clear();
    d.resize(n + 1, infinity);
    d[s] = 0;
    queue<int> q;
    q.push(s);
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (matrix_1d_t::iterator i = graph.impl[u].begin(); 
             i != graph.impl[u].end(); ++i) {
            int v = i->second;
            if (d[v] == infinity) {
                d[v] = d[u] + 1;
                q.push(v);
            }
        }
    }
}

unsigned long long
checksum_serial() {
    unsigned long long c = 0;
    for (int i = 1; i < n+1; ++i) {
        c += d[i];
    }
    return c;
}

int
main() {
    read_input();
    for (int i = 0; i < r; ++i) {
        serial_bfs(sources[i]);
        unsigned long long c = checksum_serial();
        cout<<d[sources[i]]<<" "<<c<<"\n";
    }
}
