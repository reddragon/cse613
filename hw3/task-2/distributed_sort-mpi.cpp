#include <mpi.h>

int 
main(int argc, char** argv) {
    int p, myrank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    printf("My rank is %d/%d\n", myrank, p);
    MPI_Finalize();
}
