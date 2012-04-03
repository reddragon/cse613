CXXFLAGS := $(CXXFLAGS) -O2 -Wall

all: serial parallel parallel_d parallel_h

serial: serial_bfs.cpp
	$(CXX) -o serial serial_bfs.cpp $(CXXFLAGS)

parallel: parallel_bfs.cpp
	cilk++ -o parallel parallel_bfs.cpp $(CXXFLAGS)

parallel_d: parallel_bfs.cpp
	cilk++ -o parallel_d parallel_bfs.cpp $(CXXFLAGS) -DPARTD

parallel_h: parallel_bfs.cpp
	cilk++ -o parallel_h parallel_bfs.cpp $(CXXFLAGS) -DPARTH

clean:
	rm -f serial parallel parallel_d parallel_h
