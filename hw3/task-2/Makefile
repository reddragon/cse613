CXXFLAGS  := $(FLAGS) -O2 -Wall
CILKFLAGS := -lcilkutil
CILKXX := cilk++
MPICXX := mpicc

all: common.so distributed_sort-mpi

common.so: ../task-1/common.hpp ../task-1/common.cpp
	$(CILKXX) -m64 -fPIC -shared -o common.so ../task-1/common.cpp

distributed_sort-mpi: distributed_sort-mpi.cpp
	$(MPICXX) distributed_sort-mpi.cpp -o distributed_sort-mpi -lcommon

clean:
	rm -f common.so distributed_sort-mpi

