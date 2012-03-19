XFLAGS    := $(CXXFLAGS) -Wall -O2
CXXFLAGS  := $(XFLAGS)
CILKFLAGS := $(XFLAGS)
CILKXX    := cilk++
PAPIFLAGS := /opt/apps/papi/4.1.2.1/lib/libpapi.a -I /opt/apps/papi/4.1.2.1/include/ -DWITH_PAPI

ifeq ($(CILKXX), cilk++)
	CILKFLAGS := $(XFLAGS) -lcilkutil
endif

all: a.out b.out c.out e.out check.out

a.out: a/main.cpp include/common.hpp
	$(CXX) -o a/a.out a/main.cpp $(CXXFLAGS)

b.out: b/main.cpp include/common.hpp
	$(CILKXX) -o b/a.out b/main.cpp $(CILKFLAGS)

c.out: c/main.cpp include/common.hpp
	$(CILKXX) -o c/a.out c/main.cpp $(CILKFLAGS)

e.out: e/main.cpp include/common.hpp
	$(CILKXX) -o e/a.out e/main.cpp $(CILKFLAGS)

fa.out: a/main.cpp include/common.hpp
	$(CXX) -o f/fa.out a/main.cpp $(CXXFLAGS) $(PAPIFLAGS)

fc.out: c/main.cpp include/common.hpp
	$(CXX) -o f/fc.out c/main.cpp $(CXXFLAGS) $(PAPIFLAGS)

check.out: check/main.cpp
	g++ -o check.out check/main.cpp $(CXXFLAGS)

test:
	make

clean:
	rm a/a.out b/a.out c/a.out e/a.out check.out > /dev/null 2>&1
