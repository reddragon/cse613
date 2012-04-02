#include <iostream>
#include <stdio.h>
#include <map>
#include <vector>
#include <queue>
#include <algorithm>
#include <assert.h>
#include <math.h>
#include <cilk.h>
#include <cilk_mutex.h>

using namespace std;

typedef map<int, int> matrix_1d_t;
typedef vector<matrix_1d_t> matrix_2d_t;

#define cfor for

struct node_pair {
    int p, u;
    node_pair(int P = -1, int U = -1)
        : p(P), u(U)
    { }
};

typedef vector<int> vi_t;
typedef vector<vi_t> vvi_t;
typedef vector<node_pair> vnp_t;
typedef vector<vnp_t> vvnp_t;


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

struct segment_t {
    vnp_t *q;
    int i, j;

    segment_t(vnp_t *Q, int I, int J)
        : q(Q), i(I), j(J)
    { }
};

graph_t graph;
vi_t d, sources;
int n, m, r;
int p; // # of processing cores
vvnp_t Qin, Qout;

// Each entry of QinStart[i] is an index of the first un-processed
// element of the queue at Qin[i].
vi_t QinStart;

// The first index 'i' in Qin where Qin[i] is non-empty (or one index
// before it).
int QminNonEmpty = 0;

int infinity = -1;
cilk::mutex ns_mutex;

#if defined PARTD
vi_t parent;
#endif


void
initialize() {
}

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

segment_t
next_segment(int size) {
    segment_t seg(NULL, 0, 0);

    ns_mutex.lock();

    int i;
    for (i = QminNonEmpty; i < QinStart.size(); ++i) {
        if (QinStart[i] < Qin[i].size()) {
            int m = Qin[i].size() - QinStart[i];
            m = std::min(size, m);
            seg.q = &(Qin[i]);
            seg.i = QinStart[i];
            seg.j = seg.i + m;
            QinStart[i] += m;
            break;
        }
    }
    QminNonEmpty = i;

    ns_mutex.unlock();

    return seg;
}

void
parallel_bfs_thread(int k, int size) {
    segment_t seg = next_segment(size);
    while (seg.q) {
        while (seg.i != seg.j) {
            node_pair np = (*seg.q)[seg.i++];
            int u = np.u;
            int p = np.p;

#if defined PARTD
            if (parent[u] == p) {
#endif

                for (matrix_1d_t::iterator i = graph.impl[u].begin(); 
                     i != graph.impl[u].end(); ++i) {
                    int v = i->second;
                    if (d[v] == infinity) {
                        d[v] = d[u] + 1;
                        Qout[k].push_back(node_pair(u, v));
#if defined PARTD
                        parent[v] = u;
#endif
                    }
                }
#if defined PARTD
            }
#endif
        }
        seg = next_segment(size);
    }
}

int
sizeofQin() {
    int sz = 0;
    for (int i = 0; i < Qin.size(); ++i) {
        sz += Qin[i].size();
    }
    return sz;
}

void
parallel_bfs(int s) {
    d.clear();
    d.resize(n + 1, infinity);
    d[s] = 0;

    Qin.clear();
    Qout.clear();
    QinStart.clear();

#if defined PARTD
    parent.clear();
    parent.resize(n+1, -1);
    parent[s] = -1;
#endif

    Qin.resize(p);
    Qout.resize(p);
    QinStart.resize(p, 0);

    QminNonEmpty = 0;

    assert(!Qin.empty());

    Qin[0].push_back(node_pair(-1, s));
    int nseg = p; // TODO - FIXME

    while (QminNonEmpty < Qin.size()) {
        int size = ceil(sizeofQin() / nseg);
        if (!size) {
            break;
        }

        cfor(int i = 0; i < p; ++i) {
            parallel_bfs_thread(i, size);
        }
        Qin.swap(Qout);
        QminNonEmpty = 0;
        QinStart.clear();
        QinStart.resize(p, 0);
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
cilk_main() {
    read_input();
    for (int i = 0; i < r; ++i) {
        parallel_bfs(sources[i]);
        unsigned long long c = checksum_serial();
        cout<<c<<"\n";
    }
}
