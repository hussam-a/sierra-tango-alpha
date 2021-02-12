#ifndef  CONSTRAINTS_H
#define  CONSTRAINTS_H
#include <fstream>
#include <fstream>
#include <regex>
#include <iomanip>
#include <cstdlib>
#include <iostream>
#include <unordered_map>

using namespace std;

#define MIN_NET_CAP 0.1
#define MAX_NET_CAP 1

#define MIN_INPUT_TIMING    0.1
#define MAX_INPUT_TIMING    1



//TODO: Include Dag.h and remove the typedef

using namespace std;
typedef float cap_t;
typedef unordered_map<string, cap_t> cap_map_t;
void write_random_net_cap(const vector<string>& net_names, ostream& outs);
void extract_nets(ifstream& ins);
void parse_net_input(vector<string>& input_names, vector<string>& wire_names, ifstream& netlist_stream);

#endif
