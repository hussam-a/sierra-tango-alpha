# sierra-tango-alpha
sierra-tango-alpha is a library of open-source binaries for static timing analysis tasks.

## Source Files
```
src
├── cpm.cpp
├── cpm.h
├── DAG.cpp
├── DAG.h
├── gen-constraints.cpp
├── gen-constraints.h
├── netlist.cpp
├── netlist.h
├── parser.cpp
├── parser.h
├── path_finding.cpp
├── path_finding.h
├── path-lister.cpp
├── slacks.cpp
├── sta.cpp
└── violations.cpp
```
## Binaries

1. ***path-lister***: A module to list all paths in a given Verilog netlist.
2. ***gen-constraints***: A module to generate random net capacitances, input time delays and clock constraints given a Verilog netlist.
3. ***sta***: A tool for static timing analysis and reporting the delay across all the paths
4. ***violations***: A tool to report timing violations such as setup and hold violations
5. ***slacks***: A tool to compute the RAT, AAT, and slacks for all nodes in the timing graph and exports it to the **DOT graph descriptive language**.

## Dependecies

1. Boost Library for the Liberty parser

2. **Liberty Parser**: http://vlsicad.ucsd.edu/~sharma/Research/software/liberty_parser/

3. Graph Rendering Tool Tool (**GraphViz** would do it for us):
```
sudo apt-get install graphviz
```

4. **tsort**: Linux Topological Sorting Tool (Installed and in Path): https://linux.die.net/man/1/tsort


## Building the Library

To build the library, we use make. Make sure make is installed with **gcc>=4.9** (**for full-support of regular expressions**)

You can choose to build specific modules, by just doing make followed by either module name of the ones mentioned above. That is ``make path-lister``, ``make gen-constraints``, ``make sta``, ``make violations``, or ``make slacks``

To build all binaries: `` make all ``. To clean all builds and remove binaries, ``make clean``

### Testing the library

The library comes with some verilog netlist files for testing. To run all tests:
```
cd tests
chmod +x ./constraints.sh
chmod +x ./project.sh
./constraints.sh
./project.sh
```

### Module Command Line arguments

#### Module: gen-constraints

To build the module
```
make gen-constraints
```
Command Line Arguments:
```
./bin/gen-constraints [NETLIST_INPUT_FILE] [CAPACITANCE_FILE] [DELAY_FILE]
```

#### Command Line Arguments
Except for the ``gen-constraints`` module, any module expects these files as inputs:

```
MODULE [LIBERTY_FILE_PATH] [NET_LIST] [CAPACITANCE_FILE] [CONSTRAINT_FILE] [CLK_SKEW_FILE] [PATH_REPORT_OUTPUT_FILE] [VIOLATIONS_FILE_PATH]
```
