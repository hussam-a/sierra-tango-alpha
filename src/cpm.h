#ifndef CPM_H
#define CPM_H

#include <string>
#include <vector>
#include "path_finding.h"
#include "../libertyparser-1.0/src/liberty.h"

using namespace std;
using namespace utils;
using namespace liberty;

struct analysis_node_t{
  string name;
  cap_t output_cap;
  delay_t node_delay;
  delay_t input_transition_time;
  std::vector<delay_t> input_transition_time_list;
};

typedef std::unordered_map<std::string, analysis_node_t> path_analysis_t;

delay_t get_transtion_time (std::string name, delay_t input_transition_time,cap_t output_cap,  liberty::Library &l);
delay_t get_cell_time(std::string name, delay_t input_transition_time,cap_t output_cap,  liberty::Library &l);
cap_t get_input_pin_cap(std::string name,  liberty::Library &l);
void print_path_report_header(ostream& outs);
void print_path_report_footer(ostream& outs,  delay_t& total_path_delay);
void print_node_report(ostream& outs, node* node_report_ptr, analysis_node_t& r,delay_t path_delay_so_far);
delay_t analyzePaths( Library &l, DAG &g, path analysis_path, ostream& outs, ofstream & violate);
void analyzePrintPathReports( liberty::Library &l, DAG &g, vector<path>& all_paths, ostream& outs,ofstream & violate);
void Identitfy_violation( path_analysis_t analysis, PATH_T pathtype, delay_t path_delay,ofstream & violate,DAG &g);
pair<delay_t,path> getCriticalPath( DAG &g,ostream& outs);
cap_t getAssignOutCapacitance(node* in_node,analysis_node_t& r,  Library &l, DAG &g);
cap_t getAssignInputTransition(node* in_node,analysis_node_t& r,  Library &l, DAG& g);
delay_t getDelayConstraint( string& node_name, DAG& g);
void CPM(Library &l,DAG& g);
void print_Cell (ostream& outs, node* node_report_ptr, analysis_node_t& r, delay_t path_delay_so_far);
#endif
