all: a b c e check

a:
	CXX -o a a/main.cpp -O2 $(CFLAGS)

b:
	CXX -o b b/main.cpp -O2 $(CFLAGS)

c:
	CXX -o c c/main.cpp -O2 $(CFLAGS)

e:
	CXX -o e e/main.cpp -O2 $(CFLAGS)

check:
	g++ -o check check/main.cpp -O2

test:
	make

clean:
	rm decafch lex.yy.c decaf.tab.c decaf.tab.h decaf.output > /dev/null 2>&1
