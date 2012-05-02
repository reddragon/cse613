#include <mpi.h>
#include <stdlib.h>

int*
pivot_selection(int l, int* A) {
    for (int i = 0; i < l; i++)
        printf("%d ", A[i]);
    printf("\n");
}

int 
dsort_slave(int r) {
    MPI_Status ms;
    // Receive its share of the work
    // Receive the size of the array that is going to be received
    // TODO: Do we really need to do this?
    int count;
    MPI_Recv(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &ms);
    
    int* A = new int[count];
    MPI_Recv(A, count, MPI_INT, 0, 0, MPI_COMM_WORLD, &ms);

    // TODO:
    // Do pivot_selection
    int* pivots = pivot_selection(count, A);

    // Send pivots across
    // Receive global pivots
    // Do local bucketing
    // Distribute local buckets
    delete A;
}

int
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
        MPI_Send(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &ms);

        // Send array from A[from] to A[upto] (both inclusive)
        MPI_Send((void*)A, count, MPI_INT, i, 0, MPI_COMM_WORLD, &ms);
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
    printf("My rank is %d/%d\n", myrank, p);

    if (myrank == 0) {
        // Master Process
        // Read data
        
    }
    MPI_Finalize();
}
