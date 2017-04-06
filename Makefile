CPP = g++
COPT = -std=c++11

all:	p01.o scopinfo.o normalization.o ast.o lex_scheduling.o serial_codegen.o scop.o utility.o options.o codegen.o debug.o tiling.o
	$(CPP) $(COPT) -o simplePoly p01.o scopinfo.o normalization.o ast.o lex_scheduling.o serial_codegen.o scop.o utility.o options.o codegen.o debug.o tiling.o -L/usr/local/lib -L/usr/local/lib -lisl -lpet

p01.o:	p01.cc scopinfo.hh
	$(CPP) $(COPT) -c p01.cc

scopinfo.o:	scopinfo.cc scopinfo.hh
	$(CPP) $(COPT) -c scopinfo.cc

normalization.o:	normalization.cc normalization.hh
	$(CPP) $(COPT) -c normalization.cc

ast.o:	ast.cc ast.hh
	$(CPP) $(COPT) -c ast.cc

lex_scheduling.o:	lex_scheduling.cc lex_scheduling.hh
	$(CPP) $(COPT) -c lex_scheduling.cc

serial_codegen.o:	serial_codegen.cc ast.hh
	$(CPP) $(COPT) -c serial_codegen.cc

scop.o:	scop.cc scop.hh
	$(CPP) $(COPT) -c scop.cc

utility.o:	utility.cc utility.hh
	$(CPP) $(COPT) -c utility.cc

options.o:	options.cc options.hh
	$(CPP) $(COPT) -c options.cc

codegen.o:	codegen.cc codegen.hh
	$(CPP) $(COPT) -c codegen.cc

debug.o:	debug.cc debug.hh
	$(CPP) $(COPT) -c debug.cc

tiling.o:	tiling.cc tiling.hh
	$(CPP) $(COPT) -c tiling.cc


clean:
	rm *.o simplePoly
