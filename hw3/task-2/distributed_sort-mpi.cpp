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

int RANK = -1;

#define dprintf(FMT,ARGS...) fprintf(stderr, "%d::" FMT, RANK, ARGS);

template <typename Iter>
bool
is_sorted(Iter f, Iter l) {
    Iter next = f;
    while (f != l) {
        ++next;
        if (next != l && *f > *next) {
            return false;
        }
        f = next;
    }
    return true;
}

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
    dprintf("MPI_send_data_t_array(n: %d, buff: %p)\n", n, buff);
    int ret;
    ret = MPI_Send(&n, 1, MPI_LONG_LONG_INT, rank, 0, MPI_COMM_WORLD);
    assert(ret == MPI_SUCCESS);

    // Send buffer
    ret = MPI_Send((void*)(buff), n, MPI_LONG_LONG_INT, rank, 0, MPI_COMM_WORLD);
    assert(ret == MPI_SUCCESS);
    return ret;
}

MPI_Status
MPI_receive_data_t_array(vector<data_t> &buff, int rank) {
    dprintf("MPI_receive_data_t_array(buff.size(): %d)\n", buff.size());
    long long int count;
    MPI_Status ms;
    int ret;
    ret = MPI_Recv(&count, 1, MPI_LONG_LONG_INT, rank, 0, MPI_COMM_WORLD, &ms);
    assert(ret == MPI_SUCCESS);

    dprintf("MPI_receive_data_t_array::count: %d\n", count);

    buff.resize(count);
    data_t* A = &*buff.begin();
    ret = MPI_Recv(A, count, MPI_LONG_LONG_INT, rank, 0, MPI_COMM_WORLD, &ms);
    assert(ret == MPI_SUCCESS);
    return ms;
}

vector<data_t>*
pivot_selection_slave(size_t l, data_t *A, int npivots) {
    dprintf("pivot_selection_slave(l: %d, A: %p, npivots: %d)\n", l, A, npivots);
    /*
    size_t rsz = 12 * log(l);
    rsz = rsz >= l ? l : rsz;
    vector<data_t> pivots(rsz);
    sample_input(A, A+l, pivots.begin(), pivots.end());
    */

    // Use Shared-Memory Sort
    parallel_randomized_looping_quicksort_CPP(A, 0, l-1);
    assert(is_sorted(A, A + l));

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
    dprintf("pivot_selection_master(n: %d, A: %p, p: %d, q: %d)\n", n, A, p, q);
    MPI_Status ms;
    int code;
    // The place to put together all pivots

    std::vector<data_t>* piv = 
        // The pivots generated by the master    
        pivot_selection_slave((int)(n*1.0/p), A, q-1);

    std::vector<data_t> &pivots = *piv;
    std::vector<data_t> sp;
    sp.resize(q-1);

    for (int i = 1; i < p; i++) {
        // Receive keys_with[i] pivots from each process
        data_t* B = &(sp[0]);

        // Fetch the pivots from the slave
        // code = MPI_Recv(B, q-1, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD, &ms);
        code = MPI_receive_data_t_array(sp, i);
        assert(code == MPI_SUCCESS);
        // Append to the pivots received so far
        pivots.insert(pivots.end(), sp.begin(), sp.end());
    }

    // Sort the pivots received so far
    int l = (int)(pivots.size());
    parallel_randomized_looping_quicksort_CPP(&(pivots[0]), 0, pivots.size() - 1);
    assert(is_sorted(pivots.begin(), pivots.end()));

    std::vector<data_t> *ret = new std::vector<data_t>;
    int jmp = l/(p-1);

    jmp = jmp < 1 ? 1 : jmp;
    for (int i = 0; i < l; i += jmp) {
        ret->push_back(pivots[i]);
    }

    // Cleanup
    delete piv;

    return ret;
}

void
send_global_pivots(vector<data_t> *global_pivots, int p) {
    dprintf("send_global_pivots(global_pivots->size(): %d, p: %d\n", global_pivots->size(), p);
    for (int i = 1; i < p; i++) {
        MPI_send_data_t_array(p-1, &(*global_pivots->begin()), i);
    }
}

void
dsort_slave(int r, int p, int q) {
    dprintf("dsort_slave(r: %d, p: %d, q: %d)\n", r, p, q);
    MPI_Status ms;
    vector<data_t> buffer;

    // Receive its share of the work
    MPI_receive_data_t_array(buffer, 0);

    data_t* A = &*buffer.begin();

    // Do pivot_selection
    vector<data_t>* pivots = pivot_selection_slave(buffer.size(), A, q-1);

    // Post-Condition: A is now sorted!
    dprintf("pivots (pointer): %p\n", pivots);
    assert(is_sorted(A, A+buffer.size()));
    
    // Send pivots across to master
    MPI_send_data_t_array(pivots->size(), &*pivots->begin(), 0);
    // MPI_Send((void*)(&*pivots->begin()), pivots->size(), MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD);

    // Receive (p-1) global pivots from master
    // pivots->resize(p);
    // MPI_Recv(&*pivots->begin(), p-1, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD, &ms);
    MPI_receive_data_t_array(*pivots, 0);
    pivots->resize(p);
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
    std::vector<data_t> toMerge;
    for (int i = 0; i < p; ++i) {
        if (i != r) {
            std::vector<data_t> buff;
            MPI_receive_data_t_array(buff, i);
            dprintf("Processor: %d, buff size: %u", i, buff.size());
            toMerge.insert(toMerge.end(), buff.begin(), buff.end());
        } else {
            toMerge.insert(toMerge.end(), f, l);
        }
    }

    // Sort all the data using parallel shared memory sorting routine.
    parallel_randomized_looping_quicksort_CPP(&*toMerge.begin(), 0, toMerge.size() - 1);

    // Wait for all senders to have completed.
    vector<MPI_Status> statuses(p);
    MPI_Waitall(requests.size(), &*requests.begin(), &*statuses.begin());

    // Now, send out the sorted data to the master process.
    MPI_send_data_t_array(toMerge.size(), &*toMerge.begin(), 0);

    // We need not delete pivots since it will be collected on process exit.
    // delete pivots;
}

void
dsort_master(vector<data_t> &A, int p, int q) {
    dprintf("dsort_master(A.size(): %d, p: %d, q: %d)\n", A.size(), p, q);
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
        dprintf("%d keys sent to Process %d\n", i, count);

        // printf("Sent %d elements from %d to %d to processor %d\n", count, from, upto, i);
        cur += share;
    }

    // Number of pivots that I will have
    keys_with[0] = (size_t)share;
   
   // Compute global pivots
    std::vector<data_t> *global_pivots = pivot_selection_master(n, &(*A.begin()), p, q);
    
    dprintf("Sending global pivots%s\n","");
    // Send global pivots
    send_global_pivots(global_pivots, p);
    dprintf("Sent global pivots%s\n","");
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
    RANK = myrank;
    
    dprintf("Number of processes: %d\n", p);
    
    if (myrank < 0) {
        // error
        fprintf(stderr, "There was an error. RANK: %d\n", RANK);
        return 1;
    }

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
        if (p == 1) {
            parallel_randomized_looping_quicksort_CPP(&*a.begin(), 0, a.size() - 1);
        } else {
            // Call to start work.
            dsort_master(a, p, q);
        }

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
