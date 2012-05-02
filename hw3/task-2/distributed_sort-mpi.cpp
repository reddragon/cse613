#include <mpi.h>
#include <stdlib.h>
#include <math.h>

// TODO:
// All sends are receives are blocking for now
// Replace by non-blocking ones.

extern "C++" void parallel_randomized_looping_quicksort_CPP(long long int *a, size_t q, size_t r);

typedef long long int data_t;

data_t*
pivot_selection(size_t l, data_t *A) {
    size_t rsz = 12 * log(l);
    data_t *ret = new data_t[rsz];
    for (size_t i = 0; i < rsz; i++) {
        ret[i] = A[rand() % l];
    }

    // Use Shared-Memory Sort
    parallel_randomized_looping_quicksort_CPP(ret, 0, rsz-1);
    return ret;
}

void
dsort_slave(int r) {
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
    data_t* pivots = pivot_selection(count, A);

    // Send pivots across
    // Receive global pivots
    // Do local bucketing
    // Distribute local buckets
    // delete[] pivots;
}

void
dsort_master(int n, int* A, int p, int q) {
    // Distribute work
    // Trying to distribute as evenly as possible.
    double share = n * 1.0 / p, cur = 0;
    MPI_Status ms;

    for (int i = 1; i < p; i++) {
        int from = (int)cur, upto = (i == p-1 ? n - 1 : (int)(cur + share));
        int count = upto - from + 1;
        
        // Send the size of the array being sent. 
        // TODO: Do we really need to send this?
        MPI_Send(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

        // Send array from A[from] to A[upto] (both inclusive)
        MPI_Send((void*)(A + from), count, MPI_INT, i, 0, MPI_COMM_WORLD);
        printf("Sent %d elements from %d to %d to processor %d\n", count, from, upto, i);
        cur += share;
    }

    // Computing pivots for my own part
    int* pivots = pivot_selection((int)share, A);
    
    // TODO
    // Receive pivots
    // Compute global pivots
    // Send global pivots
    // Final collection
}

int 
main(int argc, char** argv) {
    int p, myrank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    if (myrank == 0) {
        // Master Process
        // Read data
        int* A = new int[100];
        for (int i = 0; i < 100; i++) {
            A[i] = i;
        }
        dsort_master(100, A, p, 4);
    } else
        dsort_slave(myrank);
    MPI_Finalize();
}
