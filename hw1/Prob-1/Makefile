CFLAGS := $(CFLAGS) -Wall

all: a.out b.out c.out e.out check.out

a.out: a/main.cpp
	$(CXX) -o a.out a/main.cpp -O2 $(CFLAGS)

b.out: b/main.cpp
	$(CXX) -o b.out b/main.cpp -O2 $(CFLAGS)

c.out: c/main.cpp
	$(CXX) -o c.out c/main.cpp -O2 $(CFLAGS)

e.out: e/main.cpp
	$(CXX) -o e.out e/main.cpp -O2 $(CFLAGS)

check.out: check/main.cpp
	g++ -o check.out check/main.cpp -O2

test:
	make

clean:
	rm a.out b.out c.out e.out check.out > /dev/null 2>&1
