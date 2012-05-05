#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <vector>
#include <algorithm>
#include <utility>
#include <map>
#include "../task-1/timer.hpp"

using namespace std;

// TODO:
// All sends are receives are blocking for now
// Replace by non-blocking ones.

extern "C++" void parallel_randomized_looping_quicksort_CPP(long long int *a, size_t q, size_t r);

typedef long long int data_t;
typedef pair<size_t, size_t> range_t;

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

vector<range_t>
split_into_ranges(size_t n, size_t parts) {
    range_t t, r(0, n);
    vector<range_t> ret;
    size_t jmp = 1.0 * n/parts;
    jmp = jmp < 1 ? 1 : jmp;
    ret.push_back(r);
    for (size_t i = 1; i < parts; ++i) {
        t = ret.back();
        r = make_pair(t.first, t.first + jmp);
        t.first += jmp;
        ret.pop_back();
        ret.push_back(r);
        ret.push_back(t);
    }
    return ret;
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
    int jmp = l/(npivots + 1);

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
    pivot_selection_slave(n, A, q-1);

    std::vector<data_t> &pivots = *piv;
    std::vector<data_t> sp;
    sp.resize(q-1);

    for (int i = 1; i < p; i++) {
        // Receive keys_with[i] pivots from each process
        data_t* B = &(sp[0]);

        // Fetch the pivots from the slave
        // code = MPI_Recv(B, q-1, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD, &ms);
        MPI_receive_data_t_array(sp, i);
        // assert(code == MPI_SUCCESS);
        // Append to the pivots received so far
        pivots.insert(pivots.end(), sp.begin(), sp.end());
    }

    // Sort the pivots received so far
    int l = (int)(pivots.size());
    parallel_randomized_looping_quicksort_CPP(&(pivots[0]), 0, pivots.size() - 1);
    assert(is_sorted(pivots.begin(), pivots.end()));

    std::vector<data_t> *ret = new std::vector<data_t>;
    int jmp = l/p;

    jmp = jmp < 1 ? 1 : jmp;
    for (int i = jmp; i < l; i += jmp) {
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
collect_buckets(std::vector<data_t>* f, int p) {
    std::vector<data_t>* temp = new std::vector<data_t>;
    dprintf("collect_buckets::current bucket size: %d\n", f->size());
    for (int i = 1; i < p; i++) {
        MPI_receive_data_t_array(*temp, i);
        dprintf("Received process %d's bucket of size: %d\n", i, temp->size());
        f->insert(f->end(), temp->begin(), temp->end());
    }
}


std::vector<data_t>*
local_bucketing(int r, int p, data_t* A, int buff_sz, std::vector<data_t>* pivots) {
    dprintf("Starting with local bucketing(r:%d, p:%d, A:%p, buff_sz:%d, pivots:%p)\n", r, p, A, buff_sz, pivots);

    if (pivots->size() != p) {
        dprintf("pivots->size(): %d\n", pivots->size());
        assert(false);
    }

    if  (buff_sz && !((*pivots)[p-1] > A[buff_sz-1])) {
        dprintf("Last element of pivots is not > last element of A\n", "");
        assert(false);
    }

    // Do local bucketing & Distribute local buckets
    std::vector<MPI_Request> requests(p);
    std::vector<MPI_Request> creqs(p);
    std::vector<data_t> counts(p);
    
    dprintf("-----------------Chunk Size: %d-------------\n", buff_sz);
    data_t *start = A, *f = NULL, *l = NULL;
    for (int i = 0; i < p; ++i) {
        dprintf("start: %lld, pivots[%d]: %lld\n", *start, i, (*pivots)[i]);
        data_t *pos = std::lower_bound(start, A + buff_sz, (*pivots)[i]);
        if (i == r) {
            f = start;
            l = pos;
            // dprintf("Part %d: (%d - %d), length:%d\n", i, f-A, (l-1)-A,l-f);
            dprintf("Sending to %d count: %d\n", i, l-f);
        } else {
            long long int count = pos-start;
            counts[i] = count;
            // dprintf("Part %d: (%d - %d), length:%d\n", i, (start)-A, (pos-1)-A, count);
            dprintf("-----------------L:%d---------------\n", count);
            dprintf("Sending to %d count: %d\n", i, count);
            MPI_Isend(&counts[i], 1, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD, &creqs[i]);
            MPI_Isend(start, (pos - start), MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD, &requests[i]);
        }
        start = pos;
    }

    // Receive keys from p-1 different processes.
    std::vector<data_t>* toMerge = new std::vector<data_t>;
    for (int i = 0; i < p; ++i) {
        if (i != r) {
            std::vector<data_t> buff;
            MPI_receive_data_t_array(buff, i);
            dprintf("Processor: %d, buff size: %u\n", i, buff.size());
            toMerge->insert(toMerge->end(), buff.begin(), buff.end());
        } else {
            toMerge->insert(toMerge->end(), f, l);
        }
    }

    // Sort all the data using parallel shared memory sorting routine.
    parallel_randomized_looping_quicksort_CPP(&*toMerge->begin(), 0, toMerge->size() - 1);

    // Wait for all senders to have completed.
    vector<MPI_Status> statuses(p);
    requests.erase(requests.begin() + r);
    creqs.erase(creqs.begin() + r);

    MPI_Waitall(requests.size(), &*requests.begin(), &*statuses.begin());

    dprintf("Done with local bucketing(r:%d, p:%d, A:%p, buff_sz:%d, pivots:%p, toMerge->size():%u)\n", r, p, A, buff_sz, pivots, toMerge->size());
    MPI_Waitall(creqs.size(), &creqs[0], &statuses[0]);
    return toMerge;
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
    assert(is_sorted(pivots->begin(), pivots->end()));

    std::vector<data_t>* ret;
    // ret = new std::vector<data_t>;
    // ret->push_back(1);
    
    ret = local_bucketing(r, p, A, (int)(buffer.size()), pivots);   
    
    // Now, send out the sorted data to the master process.
    MPI_send_data_t_array(ret->size(), &(*(ret->begin())), 0);
    
    // We need not delete pivots since it will be collected on process exit.
    // delete pivots;
}

void
dsort_master(vector<data_t> &A, int p, int q) {
    dprintf("dsort_master(A.size(): %d, p: %d, q: %d)\n", A.size(), p, q);
    // Distribute work
    // Trying to distribute as evenly as possible.
    size_t n = A.size();
#if 0
    double share = n * 1.0 / p, cur = 0;
    cur = (int)share;

    for (int i = 1; i < p; i++) {
        // The first (int)share keys will be sorted by
        // the master
        int from = (int)cur, upto = (i == p-1 ? n - 1 : (int)(cur + share - 1));
        long long int count = upto - from + 1;

        data_t *buff = &*A.begin();

        // Send array from A[from] to A[upto] (both inclusive)
        MPI_send_data_t_array(count, (buff + from), i);
        dprintf("%d keys sent to Process %d\n", count, i);

        // printf("Sent %d elements from %d to %d to processor %d\n", count, from, upto, i);
        cur += share;
    }

    // Compute global pivots
    std::vector<data_t> *global_pivots = pivot_selection_master((int)share, &(*A.begin()), p, q);
#else
    vector<range_t> ranges = split_into_ranges(n, p);
    data_t *buff = &A[0];

    for (size_t i = 1; i < ranges.size(); ++i) {
        range_t r = ranges[i];

        // Send array [ A[r.first] .. A[r.second] ) to the i^th process.
        MPI_send_data_t_array(r.second - r.first, buff + r.first, i);
        dprintf("%d keys sent to Process %d\n", r.second - r.first, i);
    }

    // Compute global pivots
    std::vector<data_t> *global_pivots = 
        pivot_selection_master(ranges[0].second, buff, p, q);
#endif

    dprintf("Sending global pivots%s\n","");
    // Send global pivots
    send_global_pivots(global_pivots, p);
    dprintf("Sent global pivots%s\n","");
    // Final collection

    global_pivots->resize(p);
    // (*global_pivots)[p-1] = A[((int)share)-1] + 1;
    (*global_pivots)[p-1] = A[ranges[0].second - 1] + 1;

    std::vector<data_t>* ret;
    // ret = new std::vector<data_t>;
    // ret->push_back(1);
    // ret = local_bucketing(0, p, &(*A.begin()), (int)(share), global_pivots);
    ret = local_bucketing(0, p, buff, ranges[0].second, global_pivots);

    dprintf("Receiving final buckets from all\n", "");
    collect_buckets(ret, p);
    dprintf("Received buckets from all, ret->size(): %u\n", ret->size());
    assert(is_sorted(ret->begin(), ret->end()));
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

        dprintf("Read in %d records from input file\n", n);
        Timer t;
        t.start();
        if (p == 1) {
            parallel_randomized_looping_quicksort_CPP(&*a.begin(), 0, a.size() - 1);
        } else {
            // Call to start work.
            dsort_master(a, p, q);
        }

        double total_sec = t.stop();
        
        /*
        for (long long int i = 0; i < n; ++i) {
            printf("%lld\n", a[i]);
        }
        */

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
