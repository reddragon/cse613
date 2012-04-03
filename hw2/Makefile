CXXFLAGS := $(FLAGS) -O2 -Wall

all: serial parallel parallel_d parallel_h

serial: serial_bfs.cpp
	$(CXX) -o serial serial_bfs.cpp $(CXXFLAGS) -Wno-unused-result

parallel: parallel_bfs.cpp
	cilk++ -o parallel parallel_bfs.cpp $(CXXFLAGS) $(CILKFLAGS) 

parallel_d: parallel_bfs.cpp
	cilk++ -o parallel_d parallel_bfs.cpp $(CXXFLAGS) $(CILKFLAGS) -DPARTD

parallel_h: parallel_bfs.cpp
	cilk++ -o parallel_h parallel_bfs.cpp $(CXXFLAGS) $(CILKFLAGS) -DPARTH

pdf: hw2.tex
	pdflatex hw2.tex

clean:
	rm -f serial parallel parallel_d parallel_h
