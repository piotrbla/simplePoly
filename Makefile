CPP = g++
COPT = -std=c++11

all:	p01.o scopinfo.o normalization.o ast.o
	$(CPP) $(COPT) -o simplePoly p01.o scopinfo.o normalization.o ast.o -L/usr/local/lib -L/usr/local/lib -lisl -lpet

p01.o:	p01.cc scopinfo.hh
	$(CPP) $(COPT) -c p01.cc

scopinfo.o:	scopinfo.cc scopinfo.hh
	$(CPP) $(COPT) -c scopinfo.cc

normalization.o:	normalization.cc normalization.hh
	$(CPP) $(COPT) -c normalization.cc

ast.o:	ast.cc ast.hh
	$(CPP) $(COPT) -c ast.cc

clean:
	rm *.o simplePoly
