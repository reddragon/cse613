all: serial parallel parallel_d

serial: serial_bfs.cpp
	$(CXX) -o serial serial_bfs.cpp -O2 -Wall

parallel: parallel_bfs.cpp
	cilk++ -o parallel parallel_bfs.cpp -O2 -Wall

parallel_d: parallel_bfs.cpp
	cilk++ -o parallel_d parallel_bfs.cpp -O2 -Wall -DPARTD

clean:
	rm -f serial parallel
