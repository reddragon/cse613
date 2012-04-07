CXXFLAGS  := $(FLAGS) -O2 -Wall
CILKFLAGS := -lcilkutil

all: serial parallel parallel_d parallel_h parallel_cv parallel_h_cv timer.hpp

serial: serial_bfs.cpp
	$(CXX) -o serial serial_bfs.cpp $(CXXFLAGS)

parallel: parallel_bfs.cpp
	cilk++ -o parallel parallel_bfs.cpp $(CXXFLAGS) $(CILKFLAGS) 

parallel_d: parallel_bfs.cpp
	cilk++ -o parallel_d parallel_bfs.cpp $(CXXFLAGS) $(CILKFLAGS) -DPARTD

parallel_h: parallel_bfs.cpp
	cilk++ -o parallel_h parallel_bfs.cpp $(CXXFLAGS) $(CILKFLAGS) -DPARTH

parallel_cv: parallel_bfs.cpp
	cilk++ -o parallel_cv parallel_bfs.cpp $(CXXFLAGS) $(CILKFLAGS) -DCILKVIEWPLOT	

parallel_h_cv: parallel_bfs.cpp
	cilk++ -o parallel_h_cv parallel_bfs.cpp $(CXXFLAGS) $(CILKFLAGS) -DPARTH -DCILKVIEWPLOT

sample02:
	qsub qrun-sample02.sh

sample02-dev:
	qsub qrun-sample02-dev.sh

turnin:
	qsub qrun-turnin01-dev.sh
	qsub qrun-turnin02-dev.sh
	qsub qrun-turnin03-dev.sh
	qsub qrun-turnin04-dev.sh

pdf: hw2.tex
	pdflatex hw2.tex

clean:
	rm -f serial parallel parallel_d parallel_h
