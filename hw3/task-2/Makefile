CXX := g++
CXXFLAGS  := $(FLAGS) -O2 -Wall
CILKFLAGS := -lcilk_main -lcilkrts -lcilkutil
CILKXX := cilk++
MPICXX := mpicc
MPICXXFLAGS := -w1 -O2
CILKHOME := ~/cilk

all: libcommon.so distributed_sort-mpi rand

libcommon.so: ../task-1/common.hpp ../task-1/common.cpp
	$(CILKXX) -m64 -fPIC -shared -o libcommon.so ../task-1/common.cpp $(CXXFLAGS)

distributed_sort-mpi: distributed_sort-mpi.cpp
	$(MPICXX) distributed_sort-mpi.cpp -o distributed_sort-mpi -L $(CILKHOME)/lib64 -L. -Wl,-rpath=. -lcommon $(CILKFLAGS) $(MPICXXFLAGS)

rand: rand.cpp
	$(CXX) $(CXXFLAGS) rand.cpp -o rand

clean:
	rm -f libcommon.so distributed_sort-mpi rand
