#include <iostream>
#include <stdio.h>
#include <map>
#include <vector>
#include <queue>
#include <assert.h>
#include "timer.hpp"

using namespace std;

typedef vector<int> matrix_1d_t;
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
        assert(u < (int)this->impl.size());
        assert(v < (int)this->impl.size());

        this->impl[u].push_back(v);
    }
};

graph_t graph;
vector<int> d, sources;
int n, m, r;
int infinity = -1;
int dmax;

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
    dmax = 0;
    vector<int> q;
    q.resize(n + 4);
    q[0] = s;
    int qi = 0;
    int qj = qi + 1;

    // queue<int> q;
    // q.push(s);
    while (/*!q.empty()*/ qi != qj) {
        // int u = q.front();
        int u = q[qi++];
        // q.pop();
        matrix_1d_t::iterator end = graph.impl[u].end();
        for (matrix_1d_t::iterator i = graph.impl[u].begin(); i != end; ++i) {
            int v = *i;
            // fprintf(stderr, "processing edge (%d, %d), d[%d] = %d, d[%d] = %d\n", u, v, u, d[u], v, d[v]);
            if (d[v] == infinity) {
                d[v] = d[u] + 1;
                dmax = std::max(dmax, d[v]);
                // q.push(v);
                q[qj++] = v;
            }
        }
    }
}

unsigned long long
checksum_serial() {
    unsigned long long c = 0;
    for (int i = 1; i < n+1; ++i) {
        c += d[i] != infinity ? d[i] : n;
    }
    return c;
}

int
main() {
    read_input();
    double total_sec = 0;
    for (int i = 0; i < r; ++i) {
        Timer t;
        t.start();
        serial_bfs(sources[i]);
        total_sec += t.stop();
        unsigned long long c = checksum_serial();
        cout<<dmax<<" "<<c<<"\n";
    }
    fprintf(stderr, "n: %d, m: %d, r: %d, time(sec): %f\n", n, m, r, total_sec/1000000.0);
}
