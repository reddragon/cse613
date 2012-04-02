all: serial parallel

serial: serial_bfs.cpp
	$(CXX) -o serial serial_bfs.cpp -O2 -Wall

parallel: parallel_bfs.cpp
	cilk++ -o parallel parallel_bfs.cpp -O2 -Wall

clean:
	rm -f serial parallel
