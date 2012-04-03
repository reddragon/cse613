#include <iostream>
#include <stdio.h>
#include <map>
#include <vector>
#include <queue>
#include <algorithm>
#include <assert.h>
#include <math.h>
#include <cilk.h>
#include <reducer_max.h>
#include <reducer_opadd.h>
#include <cilk_mutex.h>

using namespace std;

typedef map<int, int> matrix_1d_t;
typedef vector<matrix_1d_t> matrix_2d_t;

#define cfor cilk_for
#define MAX_STEAL_ATTEMPTS (2*p)
#define MIN_STEALABLE_QSIZE 32

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
        assert(u < (int)this->impl.size());
        assert(v < (int)this->impl.size());

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

typedef vector<segment_t> vseg_t;

graph_t graph;
vi_t d, sources;
int n, m, r;
int p; // # of processing cores
vvnp_t Qin, Qout;
int dmax;
vi_t pmax;

// Each entry of QinStart[i] is an index of the first un-processed
// element of the queue at Qin[i].
vi_t QinStart;

// The first index 'i' in Qin where Qin[i] is non-empty (or one index
// before it).
int QminNonEmpty = 0;

int infinity = -1;
cilk::mutex ns_mutex;

#if defined PARTH
vseg_t Qsegments;
vector<cilk::mutex> QsegMutexes;
#endif

#if defined PARTD
vi_t parent;
#endif


void
initialize() {
    p = cilk::current_worker_count();
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
    for (i = QminNonEmpty; i < (int)QinStart.size(); ++i) {
        if (QinStart[i] < (int)Qin[i].size()) {
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


#if defined PARTH
// With work stealing
void
parallel_bfs_thread(int k, int) {
    while (true) {
        QsegMutexes[k].lock();
        segment_t *pseg = &(Qsegments[k]);
        while (pseg->i != pseg->j) {
            int index = pseg->i++;
            QsegMutexes[k].unlock();
            
            node_pair np = (*(pseg->q))[index];
            int u = np.u;
            
            for (matrix_1d_t::iterator i = graph.impl[u].begin(); 
                 i != graph.impl[u].end(); ++i) {
                int v = i->first;
                // assert(false);
                // fprintf(stderr, "processing edge (%d, %d), d[%d] = %d, d[%d] = %d\n", u, v, u, d[u], v, d[v]);
                
                if (d[v] == infinity) {
                    d[v] = d[u] + 1;
                    pmax[k] = std::max(pmax[k], d[v]);
                    Qout[k].push_back(node_pair(u, v));
                }
            }

            QsegMutexes[k].lock();

        } // while()

        QsegMutexes[k].unlock();

        segment_t tmp(NULL, 0, 0);
        int steal_attempts;
        for (steal_attempts = 0; steal_attempts < MAX_STEAL_ATTEMPTS; ++steal_attempts) {
            int rk = rand() % p;
            if (rk != k) {
                QsegMutexes[rk].lock();
                // Steal half of the nodes if there are >= MIN_STEALABLE_QSIZE nodes.
                tmp = Qsegments[rk];
                if (tmp.j - tmp.i >= MIN_STEALABLE_QSIZE) {
                    int mid = tmp.i + (tmp.j - tmp.i)/2;

                    fprintf(stderr, "stole %d nodes from queue %d\n", (tmp.j - tmp.i)/2, rk);

                    Qsegments[rk].j = mid + 1;
                    tmp.i = mid;
                    QsegMutexes[rk].unlock();
                    break;
                }
                QsegMutexes[rk].unlock();
            }
        }
        if (steal_attempts == MAX_STEAL_ATTEMPTS) {
            break;
        } else {
            QsegMutexes[k].lock();
            Qsegments[k] = tmp;
            QsegMutexes[k].unlock();
        }

    } // while (true)
    QminNonEmpty = Qin.size();

}
#else
void
parallel_bfs_thread(int k, int size) {
    segment_t seg = next_segment(size);
    while (seg.q) {
        while (seg.i != seg.j) {
            // fprintf(stderr, "seg.i: %d, seg.j: %d\n", seg.i, seg.j);
            node_pair np = (*seg.q)[seg.i++];
            int u = np.u;

#if defined PARTD
            int p = np.p;
            if (parent[u] == p) {
#endif

                for (matrix_1d_t::iterator i = graph.impl[u].begin(); 
                     i != graph.impl[u].end(); ++i) {
                    int v = i->first;
                    // assert(false);
                    // fprintf(stderr, "processing edge (%d, %d), d[%d] = %d, d[%d] = %d\n", u, v, u, d[u], v, d[v]);

                    if (d[v] == infinity) {
                        d[v] = d[u] + 1;
                        pmax[k] = std::max(pmax[k], d[v]);
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
#endif

int
sizeofQin() {
    // TODO - Use cilk_for with reducers
    int sz = 0;
    for (int i = 0; i < (int)Qin.size(); ++i) {
        sz += Qin[i].size();
    }
    return sz;
}

void
parallel_bfs(int s) {
    // fprintf(stderr, "n: %d, m: %d, r: %d, p: %d\n", n, m, r, p);
    d.clear();
    d.resize(n + 1, infinity);
    d[s] = 0;
    dmax = 0;

    Qin.clear();
    Qout.clear();
    QinStart.clear();
    pmax.clear();

#if defined PARTD
    parent.clear();
    parent.resize(n+1, -1);
    parent[s] = -1;
#endif

    Qin.resize(p);
    Qout.resize(p);
    QinStart.resize(p, 0);
    pmax.resize(p, 0);

    QminNonEmpty = 0;

    assert(!Qin.empty());

    Qin[0].push_back(node_pair(-1, s));
    int nseg = p; // TODO - FIXME

#if defined PARTH
    Qsegments.clear();
    Qsegments.resize(p, segment_t(NULL, 0, 0));
    Qsegments[0] = segment_t(&(Qin[0]), 0, 1);
    QsegMutexes.resize(p);
#endif

    // fprintf(stderr, "QminNonEmpty: %d, Qin.size(): %d\n", QminNonEmpty, Qin.size());
    while (QminNonEmpty < (int)Qin.size()) {
        int size = ceil((double)sizeofQin() / nseg);
        // fprintf(stderr, "size: %d\n", size);
        if (!size) {
            break;
        }

        cfor(int i = 0; i < p; ++i) {
            parallel_bfs_thread(i, size);
        }
        Qin.swap(Qout);
        QminNonEmpty = 0;
        Qout.clear();
        Qout.resize(p);
        QinStart.clear();
        QinStart.resize(p, 0);

        cfor (int i = 0; i < p; ++i) {
#if defined PARTH
            Qsegments[i] = segment_t(&(Qin[i]), 0, Qin[i].size());
#endif
        }

        // fprintf(stderr, "QminNonEmpty: %d, Qin.size(): %d\n", QminNonEmpty, Qin.size());
    }

    cilk::reducer_max<int> rmax(0);
    int sz = (int)pmax.size();
    cfor (int i = 0; i < sz; ++i) {
        rmax = cilk::max_of(rmax, pmax[i]);
    }
    dmax = rmax.get_value();
}

unsigned long long
checksum_serial() {
    cilk::reducer_opadd< unsigned long long > chksum;
    cilk_for(int i = 1; i < n+1; i++) {
        chksum += d[i] != infinity ? d[i] : n;
    }
    return chksum.get_value( );
}

int
cilk_main() {
    initialize();
    read_input();
    for (int i = 0; i < r; ++i) {
        parallel_bfs(sources[i]);
        unsigned long long c = checksum_serial();
        cout<<dmax<<" "<<c<<"\n";
    }
}
