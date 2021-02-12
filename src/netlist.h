#ifndef NETLIST_H
#define NETLIST_H
#include "parser.h"

bool isFF(std::string s);
void fill_DAG(DAG &g);
void parse_netlist(std::ifstream &netlist, std::ifstream &cap_stream,std::ifstream &constraint_stream, std::ifstream & clk_stream, DAG &g);
void read_map_list(std::ifstream &caplist, cap_map_t &cap_map);

#endif // NETLIST_H
