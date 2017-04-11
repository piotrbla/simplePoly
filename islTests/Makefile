CPP = g++
COPT = -std=c++11

all:	p01.o 
	$(CPP) $(COPT) -o simpleTest p01.o -L/usr/local/lib -L/usr/local/lib -lisl -lpet

p01.o:	p01.cc
	$(CPP) $(COPT) -c p01.cc

clean:
	rm *.o simpleTest
