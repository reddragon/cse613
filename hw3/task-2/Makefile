CXXFLAGS  := $(FLAGS) -O2 -Wall
CILKFLAGS := -lcilkutil
CILKXX := cilk++

all: common.so 

common.so: ../task-1/common.hpp ../task-1/common.cpp
	$(CILKXX) -m32 -fPIC -shared -o common.so ../task-1/common.cpp

clean:
	rm -f common.so
