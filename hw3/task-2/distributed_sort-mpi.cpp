#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <vector>
#include <algorithm>
#include <map>
#include "../task-1/timer.hpp"

using namespace std;

// TODO:
// All sends are receives are blocking for now
// Replace by non-blocking ones.

extern "C++" void parallel_randomized_looping_quicksort_CPP(long long int *a, size_t q, size_t r);

typedef long long int data_t;

template <typename I1, typename I2>
void
sample_input(I1 s1, I1 e1, I2 s2, I2 e2) {
    size_t sz = e1 - s1;
    assert(sz > 2*(e2-s2));
    std::map<int, int> m;
    while (s2 != e2) {
        size_t ri = rand() % sz;
        if (m[ri] == 0) {
            m[ri] = 1;
            *s2++ = s1[ri];
        }
    }
}

int
MPI_send_data_t_array(long long int n, data_t *buff, int rank) {
    int ret;
    ret = MPI_Send(&n, 1, MPI_LONG_LONG_INT, rank, 0, MPI_COMM_WORLD);

    // Send buffer
    ret = MPI_Send((void*)(buff), n, MPI_LONG_LONG_INT, rank, 0, MPI_COMM_WORLD);
    return ret;
}

MPI_Status
MPI_receive_data_t_array(vector<data_t> &buff, int rank) {
    long long int count;
    MPI_Status ms;
    MPI_Recv(&count, 1, MPI_LONG_LONG_INT, rank, 0, MPI_COMM_WORLD, &ms);
    
    buff.resize(count);
    data_t* A = &*buff.begin();
    MPI_Recv(A, count, MPI_LONG_LONG_INT, rank, 0, MPI_COMM_WORLD, &ms);
    return ms;
}

vector<data_t>*
pivot_selection_slave(size_t l, data_t *A, int npivots) {

    /*
    size_t rsz = 12 * log(l);
    rsz = rsz >= l ? l : rsz;
    vector<data_t> pivots(rsz);
    sample_input(A, A+l, pivots.begin(), pivots.end());
    */

    // Use Shared-Memory Sort
    parallel_randomized_looping_quicksort_CPP(A, 0, l-1);

    std::vector<data_t> *ret = new std::vector<data_t>;
    int jmp = l/npivots;

    jmp = jmp < 1 ? 1 : jmp;
    for (int i = 0; i < l; i += jmp) {
        ret->push_back(A[i]);
    }

    return ret;
}

vector<data_t>*
pivot_selection_master(int n, data_t *A, int p, int q) {
    MPI_Status ms;
    // The place to put together all pivots
    std::vector<data_t>* pivots = new std::vector<data_t>;
    
    // The pivots generated by the master
    std::vector<data_t>* mp = pivot_selection_slave((int)(n*1.0/p), A, q-1);
    pivots->insert(pivots->end(), mp->begin(), mp->end());

    for (int i = 1; i < p; i++) {
        // Receive keys_with[i] pivots from each process
        std::vector<data_t>* sp = new std::vector<data_t>;
        sp->resize((size_t)(q-1));
        data_t* B = &*(sp->begin());

        // Fetch the pivots from the slave
        MPI_Recv(B, q-1, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD, &ms);
        // Append to the pivots received so far
        pivots->insert(pivots->end(), sp->begin(), sp->end());
        // Dispose
        delete[] sp;
    }
    
    // Sort the pivots received so far
    int l = (int)(pivots->size());
    parallel_randomized_looping_quicksort_CPP(&(*pivots->begin()), (size_t)0, (size_t)(l-1));
    
    std::vector<data_t> *ret = new std::vector<data_t>;
    int jmp = l/(p-1);

    jmp = jmp < 1 ? 1 : jmp;
    for (int i = 0; i < l; i += jmp) {
        ret->push_back(*(pivots->begin()+i));
    }

    return ret;
}

void
send_global_pivots(vector<data_t> *global_pivots, int p) {
}

void
dsort_slave(int r, int p, int q) {
    MPI_Status ms;
    vector<data_t> buffer;

    // Receive its share of the work
    MPI_receive_data_t_array(buffer, 0);

    data_t* A = &*buffer.begin();

    // Do pivot_selection
    vector<data_t>* pivots = pivot_selection_slave(buffer.size(), A, q-1);
    // Post-Condition: A is not sorted!

    // Send pivots across to master
    MPI_Send((void*)(&*pivots->begin()), pivots->size(), MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD);

    // Receive global pivots from master
    pivots->resize(p);
    MPI_Recv(&*pivots->begin(), p-1, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD, &ms);
    (*pivots)[p-1] = buffer.back() + 1;

    // Do local bucketing & Distribute local buckets
    std::vector<MPI_Request> requests(p);
    data_t *f = NULL, *l = NULL;
    data_t *start = A;
    for (int i = 0; i < p; ++i) {
        data_t *pos = std::lower_bound(A, A + buffer.size(), (*pivots)[0]);
        if (p == r) {
            f = start;
            l = pos;
        } else {
            long long int count = pos-start;
            MPI_Send(&count, 1, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD);
            MPI_Isend(start, (pos - start), MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD, &requests[i]);
        }
        start = pos;
    }

    // Receive keys from p-1 different processes.
    for (int i = 0; i < p-1; ++i) {
    }

    // Wait for all senders to have completed.
    vector<MPI_Status> statuses(p);
    MPI_Waitall(requests.size(), &*requests.begin(), &*statuses.begin());

    delete pivots;
}

void
dsort_master(vector<data_t> &A, int p, int q) {
    // Distribute work
    // Trying to distribute as evenly as possible.
    int n = A.size();
    double share = n * 1.0 / p, cur = 0;
    MPI_Status ms;
    // Number of keys with each of the p processes
    // Doing this to avoid having to receive the count again
    vector<size_t> keys_with(p);

    for (int i = 1; i < p; i++) {
        int from = (int)cur, upto = (i == p-1 ? n - 1 : (int)(cur + share - 1));
        long long int count = upto - from + 1;
        keys_with[i] = count;

        data_t *buff = &*A.begin();

        // Send array from A[from] to A[upto] (both inclusive)
        MPI_send_data_t_array(count, (buff + from), i);

        // printf("Sent %d elements from %d to %d to processor %d\n", count, from, upto, i);
        cur += share;
    }

    // Number of pivots that I will have
    keys_with[0] = (size_t)share;
   
   // Compute global pivots
    std::vector<data_t> *global_pivots = pivot_selection_master(n, &(*A.begin()), p, q);

    // Send global pivots
    send_global_pivots(global_pivots, p);

    // Final collection
}

int 
main(int argc, char** argv) {
    int p, q, myrank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    
    // FIXME: This NEEDS to be tuned.
    q = 4;

    if (myrank == 0) {
        // Master Process
        // Read data
#if 1
        long long int n;
        scanf("%lld", &n);
        std::vector<data_t> a(n);
        for (long long int i = 0; i < n; ++i) {
            scanf("%lld", &a[i]);
        }

        Timer t;
        t.start();
        // Call to start work.
        dsort_master(a, p, q);
        double total_sec = t.stop();

        for (long long int i = 0; i < n; ++i) {
            printf("%lld\n", a[i]);
        }

        fprintf(stderr, "time(sec): %f\n", total_sec/1000000.0);
#else
        data_t* A = new data_t[100];
        for (int i = 0; i < 100; i++) {
            A[i] = 100-i;
        }
        dsort_master(100, A, p, q);
#endif 
    } else {
      dsort_slave(myrank, p, q);
    }
    MPI_Finalize();
}
