CPP = g++
COPT = -std=c++11

all:	p01.o 
	$(CPP) -g $(COPT) -o simpleTest p01.o -L/usr/local/lib -L/usr/local/lib -lisl -lpet

p01.o:	p01.cc
	$(CPP) -g $(COPT) -c p01.cc

clean:
	rm *.o simpleTest
