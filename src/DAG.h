#ifndef DAG_H
#define DAG_H

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include "../libertyparser-1.0/src/liberty.h"

using namespace std;
using namespace utils;
using namespace liberty;

typedef float cap_t;
typedef float delay_t;

extern delay_t get_transtion_time (std::string name, delay_t input_transition_time,cap_t output_cap,  liberty::Library &l);
extern delay_t get_cell_time(std::string name, delay_t input_transition_time,cap_t output_cap,  liberty::Library &l);
extern cap_t get_input_pin_cap(std::string name,  liberty::Library &l);

extern delay_t get_transtion_time (std::string name, delay_t input_transition_time,cap_t output_cap,  liberty::Library &l);
extern cap_t get_input_pin_cap(std::string name,  liberty::Library &l);


class node;
class DAG;


typedef std::unordered_map<std::string, cap_t> cap_map_t;        //The capacitance list is a hashmap of the net name and the capacitance value

extern std::string NODE_T_NAMES[];
enum NODE_T {IN,OUT,CELL, FFD,FFQ,START};
extern std::string PATH_NAMES[];
enum PATH_T {NA = -1, IR, RR, RO, IO};

PATH_T get_path_type( std::string &s,  NODE_T& start_node_type,  NODE_T& end_node_type);

class edge
{
public:
    //TODO: overload the ructor to pass the net capacitance
    edge( std::string &_name,  std::string &_n):name(_name),n(_n){}
    edge( std::string &_name,  std::string &_n, cap_t &_net_capacitance):name(_name),net_capacitance(_net_capacitance),n(_n){}

    ~edge(){}
    //ToDO: pin/net name
    std::string name;
    cap_t net_capacitance;
    std::string n;
};


class node
{
public:
    node( std::string name,  NODE_T type): name(name), type(type){}
    node( std::string name,  NODE_T type,  std::string cell_type):
       name(name), type(type), cell_type(cell_type){}
    ~node(){}
    std::string name;
    NODE_T type;
    std::string cell_type;
    // TO Do- fill
    std::vector<string> in_nodes;                                               //This needs to be filled when constructing edges
    std::vector<edge> out_edges ;
    delay_t AAT;
    delay_t RAT;
    delay_t slack;
    /* All of these attributes have been moved to the analysis_node_t struct
    cap_t output_cap;
    delay_t node_delay;
    delay_t input_transition_time;
    std::vector<delay_t> input_transition_time_list;
    */
};

class DAG
{
public:
    DAG();
    ~DAG();
    std::vector<node > nodes;
    node* getNodeByName(std::string NodeName);
    //TODO: change function to get cap from hashedMap
    cap_map_t cap_map;
    cap_map_t delay_map;
    cap_map_t constraints_map;
    //TODO: get net name to join nodes
    void join(std::string edgeName, std::string n1,std::string n2);
    void outputTiming(ofstream& outs);
};

#endif
