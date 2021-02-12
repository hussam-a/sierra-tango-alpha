CPPFLAGS=-std=c++11
COMPILER=g++
LIBERTYFLAGS=-I libertyparser-1.0/src
LIBERTYLIB= libertyparser-1.0/src/*.o

path-lister: src/path-lister.cpp src/DAG.cpp src/parser.cpp src/netlist.cpp src/path_finding.cpp
	mkdir -p bin && $(COMPILER) src/DAG.cpp src/parser.cpp src/path-lister.cpp src/netlist.cpp src/path_finding.cpp $(CPPFLAGS) -o bin/path-lister

gen-constraints: src/gen-constraints.cpp
		mkdir -p bin && $(COMPILER) src/gen-constraints.cpp $(CPPFLAGS) -o bin/gen-net-cap

sta: src/netlist.cpp src/cpm.cpp src/parser.cpp src/sta.cpp src/DAG.cpp src/path_finding.cpp
	mkdir -p bin && $(COMPILER) src/netlist.cpp src/cpm.cpp src/parser.cpp src/sta.cpp src/DAG.cpp src/path_finding.cpp $(CPPFLAGS) $(LIBERTYFLAGS) -o bin/sta $(LIBERTYLIB)
	
violations: src/netlist.cpp src/cpm.cpp src/parser.cpp src/violations.cpp src/DAG.cpp src/path_finding.cpp
	mkdir -p bin && $(COMPILER) src/netlist.cpp src/cpm.cpp src/parser.cpp src/violations.cpp src/DAG.cpp src/path_finding.cpp $(CPPFLAGS) $(LIBERTYFLAGS) -o bin/violations $(LIBERTYLIB)
	
slacks: src/netlist.cpp src/cpm.cpp src/parser.cpp src/violations.cpp src/DAG.cpp src/path_finding.cpp
	mkdir -p bin && $(COMPILER) src/netlist.cpp src/cpm.cpp src/parser.cpp src/slacks.cpp src/DAG.cpp src/path_finding.cpp $(CPPFLAGS) $(LIBERTYFLAGS) -o bin/slacks $(LIBERTYLIB)

clean:
	rm bin/*
	
all: path-lister gen-constraints sta violations slacks
	
	
