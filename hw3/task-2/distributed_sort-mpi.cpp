#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <map>

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
    std::map<int, int> m;
    while (s2 != e2) {
        size_t ri = rand() % sz;
        if (m[ri] == 0) {
            m[ri] = 1;
            *s2++ = s1[ri];
        }
    }
}

vector<data_t>*
pivot_selection(size_t l, data_t *A, int npivots) {
    size_t rsz = 12 * log(l);
    rsz = rsz >= l ? l : rsz;
    vector<data_t> pivots(rsz);
    sample_input(A, A+l, pivots.begin(), pivots.end());

    // Use Shared-Memory Sort
    parallel_randomized_looping_quicksort_CPP(&*(pivots.begin()), 0, pivots.size());

    std::vector<data_t> *ret = new std::vector<data_t>;
    int jmp = l/npivots;

    jmp = jmp < 1 ? 1 : jmp;
    for (int i = 0; i < l; i += jmp) {
        ret->push_back(pivots[i]);
    }

    return ret;
}

void
dsort_slave(int r, int q) {
    MPI_Status ms;
    // Receive its share of the work
    // Receive the size of the array that is going to be received
    // TODO: Do we really need to do this?
    long long int count;
    MPI_Recv(&count, 1, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD, &ms);
    
    vector<data_t> buffer(count);
    data_t* A = &*buffer.begin();
    MPI_Recv(A, count, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD, &ms);

    // TODO:
    // Do pivot_selection
    vector<data_t>* pivots = pivot_selection(count, A, q);

    // Send pivots across
    // Receive global pivots
    // Do local bucketing
    // Distribute local buckets
    // delete[] pivots;
}

void
dsort_master(int n, data_t* A, int p, int q) {
    // Distribute work
    // Trying to distribute as evenly as possible.
    double share = n * 1.0 / p, cur = 0;
    MPI_Status ms;

    for (int i = 1; i < p; i++) {
        int from = (int)cur, upto = (i == p-1 ? n - 1 : (int)(cur + share - 1));
        int count = upto - from + 1;
        
        // Send the size of the array being sent. 
        // TODO: Do we really need to send this?
        MPI_Send(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

        // Send array from A[from] to A[upto] (both inclusive)
        MPI_Send((void*)(A + from), count, MPI_INT, i, 0, MPI_COMM_WORLD);
        // printf("Sent %d elements from %d to %d to processor %d\n", count, from, upto, i);
        cur += share;
    }

    // Computing pivots for my own part
    vector<data_t>* pivots = pivot_selection((int)share, A, q);

    // TODO
    // Receive pivots
    // Compute global pivots
    // Send global pivots
    // Final collection
}

int 
main(int argc, char** argv) {
    int p, q, myrank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    
    // TODO: This can be tuned.
    q = 4;
        
    if (myrank == 0) {
        // Master Process
        // Read data
#if 0
        long long int n;
        scanf("%lld", &n);
        std::vector<long long int> a(n);
        for (long long int i = 0; i < n; ++i) {
            scanf("%lld", &a[i]);
        }

        Timer t;
        t.start();
        // Call to start work. dsort_master()
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
     } else
        dsort_slave(myrank, q);
    MPI_Finalize();
}
