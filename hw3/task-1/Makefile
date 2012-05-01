CXXFLAGS  := $(FLAGS) -O2 -Wall
CILKFLAGS := -lcilkutil
CILKXX := cilk++

all: a b c e common.hpp

a: a.cpp
	$(CILKXX) -o a.out a.cpp $(CXXFLAGS)

b: b.cpp
	$(CILKXX) -o b.out b.cpp $(CXXFLAGS)

c: c.cpp
	$(CILKXX) -o c.out c.cpp $(CXXFLAGS)

e: c.cpp
	$(CILKXX) -o e.out c.cpp -DPARTE $(CXXFLAGS)

clean:
	rm -f a.out b.out c.out e.out
