CPP = g++

all:	p01.o scopinfo.o
	$(CPP) -o simplePoly p01.o scopinfo.o -L/usr/local/lib -L/usr/local/lib -lisl -lpet

p01.o:	p01.cc scopinfo.hh
	$(CPP) -c p01.cc

scopinfo.o:	scopinfo.cc scopinfo.hh
	$(CPP) -c scopinfo.cc
