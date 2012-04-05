CXXFLAGS := $(FLAGS) -O2 -Wall

all: serial parallel parallel_d parallel_h timer.hpp

serial: serial_bfs.cpp
	$(CXX) -o serial serial_bfs.cpp $(CXXFLAGS)

parallel: parallel_bfs.cpp
	cilk++ -o parallel parallel_bfs.cpp $(CXXFLAGS) $(CILKFLAGS) 

parallel_d: parallel_bfs.cpp
	cilk++ -o parallel_d parallel_bfs.cpp $(CXXFLAGS) $(CILKFLAGS) -DPARTD

parallel_h: parallel_bfs.cpp
	cilk++ -o parallel_h parallel_bfs.cpp $(CXXFLAGS) $(CILKFLAGS) -DPARTH

sample02:
	qsub qrun-sample02.sh

sample02-dev:
	qsub qrun-sample02-dev.sh

pdf: hw2.tex
	pdflatex hw2.tex

clean:
	rm -f serial parallel parallel_d parallel_h
