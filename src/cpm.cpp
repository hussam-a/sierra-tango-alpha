#include <vector>
#include <algorithm>
#include <string>
#include "cpm.h"
#include "netlist.h"
#include "../libertyparser-1.0/src/liberty.h"

using namespace std;
using namespace utils;
using namespace liberty;

delay_t critical_delay = 0;
path critical_path;
path_analysis_t critical_analysis;
string inn,outn;
delay_t get_transtion_time (string cell_type, delay_t input_transition_time,cap_t output_cap,  Library &l)
{
    if (cell_type.empty()) return 0;
    else
    {
        Cell c = l.getCell(cell_type);
        vector<Pin> pins = c.getPins();
        delay_t max_t = 0;
        for (Pin p: pins)
        {
            if(p.getName() > "L" )
            {

                //transistion time is max of rise and fall
                delay_t value;
                vector<Timing> tables = p.getTiming();
                Table t = tables.front().getRiseTransitionTable();
                value = t.getValue(output_cap,input_transition_time);
                max_t = max(value,max_t);
                t = tables.front().getFallTransitionTable();
                value = t.getValue(output_cap,input_transition_time);
                max_t = max(value,max_t);
            }
        }
        return max_t;
    }
}

delay_t get_cell_time(string cell_type, delay_t input_transition_time,cap_t output_cap,  Library &l)
{
    if (cell_type.empty()) return 0;
    else
    {
        Cell c = l.getCell(cell_type);
        vector<Pin> pins = c.getPins();
        delay_t max_t = 0;
        for (Pin p: pins)
        {    // o/p pin
            if(p.getName() > "L")
            {
                // max of fall,rise delay of o/p pin
                delay_t value;
                vector<Timing> tables = p.getTiming();
                Table t = tables.front().getCellFallTable();
                value = t.getValue(output_cap,input_transition_time);
                max_t = max(value,max_t);
                t = tables.front().getCellRiseTable();
                value = t.getValue(output_cap,input_transition_time);
                max_t = max(value,max_t);

            }
        }
        return max_t;
    }
}

cap_t get_input_pin_cap(string cell_type,  Library &l)
{
    if (cell_type.empty()) return 0;
    else
    {
        Cell c = l.getCell(cell_type);
        vector<Pin> pins = c.getPins();
        cap_t max_c = 0;
        for (Pin p: pins)
        {
            //max cap of o/p pins
            if(p.getName() > "L") max_c = max(p.getCapacitance(),max_c);
        }
        return max_c;
    }
}



void print_path_report_header(ostream& outs){
    static int x = 0;
    x++;
    outs << "---------------------------------------------------------------------------" << endl;
    outs << "-----------------------------Report#" << x<<  " --------------------------------------" << endl;
    outs << "---------------------------------------------------------------------------" << endl;
    outs.width(20);
    outs << left;
    outs << fixed;
    outs << "Pin";
    outs.width(20);
    outs << "Cell Type";
    outs.width(20);
    outs << "Type";
    outs.width(20);
    outs << "Increment";
    outs.width(20);
    outs << "Path Delay" << endl;
    outs << "---------------------------------------------------------------------------" << endl;
}

void print_path_report_footer(ostream& outs,  delay_t& total_path_delay){
    outs << "---------------------------------------------------------------------------" << endl;
    outs << "Data Arrival Time \t\t\t\t\t\t\t\t\t" << total_path_delay << endl;
}

void print_node_report(ostream& outs, node* node_report_ptr, analysis_node_t& r, delay_t path_delay_so_far){
    if(NODE_T_NAMES[node_report_ptr->type] =="CELL")

        print_Cell (outs,  node_report_ptr,  r,  path_delay_so_far);
       else{

    outs.width(20);
    outs << left;
    outs << fixed;
    outs << node_report_ptr->name;
    outs.width(20);
    outs << node_report_ptr->cell_type;
    outs.width(20);
    outs << NODE_T_NAMES[node_report_ptr->type];
    if(NODE_T_NAMES[node_report_ptr->type] =="IN")
        inn=node_report_ptr->name;
    if(NODE_T_NAMES[node_report_ptr->type] !="IN" && NODE_T_NAMES[node_report_ptr->type] !="FFD")
       { //inn=node_report_ptr->name;
        for( compBox &c: vecComp){
            int FF=c.type.find("DFF");
            string s= node_report_ptr->name ;int mat=s.find(c.name);
            if(FF>=0 &&   mat>=0 )
            {
                for(pin &ipin:c.inputs)
                {
                    if(ipin.pinConn==inn || ipin.pinName==inn)
                    { break;}
                }
              //  outpinname=c.outputs[0].pinName;
               // outn=c.outputs[0].pinConn;
                inn=c.outputs[0].pinConn;
            }
        }
    }
    outs.width(20);
    outs << r.node_delay;
    outs.width(20);
    outs << path_delay_so_far << endl;
    }
}

/**
 * Return path delay
 * IMPORTANT: CHANGED THIS: WHEN USING THIS, NODE DETAILS WILL BE OVERWRITTEN BY DATA SPECIFIC TO THIS PATH.
 */

delay_t analyzePaths( Library &l, DAG &g, path analysis_path, ostream& outs, ofstream & violate)
{
    // takes a path
    // loops over it and calculates the delay for each node in the path
    // print report for every node in the path

    print_path_report_header(outs);

    //analysis_node_t
    //{string name;  cap_t output_cap;  delay_t node_delay;  delay_t input_transition_time;
    //vector<delay_t> input_transition_time_list;

    //unordered list  <std::string, analysis_node_t>
    path_analysis_t analysis;
    delay_t path_delay = 0;
    //loop over the path
    //fill analysis list

    analysis.insert(make_pair(analysis_path.start, analysis_node_t()));
    analysis_node_t& rIn = (analysis.find(analysis_path.start))->second;
    rIn.name = analysis_path.start;
    rIn.input_transition_time =0;
    node& _n = *g.getNodeByName(analysis_path.start);
    rIn.node_delay = getDelayConstraint(_n.name,g);     //In case of input port, the transition is the same as the delay
    path_delay+= rIn.node_delay;
    print_node_report(outs, &_n, rIn, path_delay);

    for(string _s:analysis_path.flow){
        analysis.insert(make_pair(_s, analysis_node_t()));
        analysis_node_t& r = (analysis.find(_s))->second;
        r.name = _s;
    }
    for(string _s:analysis_path.flow)
    {
        analysis_node_t& r = (analysis.find(_s))->second;
        node& _n = *g.getNodeByName(_s);
        switch(_n.type){
        case NODE_T::IN :{                                      //If input node
            //Delay from file
            r.input_transition_time =0;
            r.node_delay = getDelayConstraint(_n.name,g);     //In case of input port, the transition is the same as the delay
        }
        case NODE_T::OUT : {
            //Previous transition time
            getAssignInputTransition(&_n, r, l,g);
            //Capacitance from fan-out nodes
            getAssignOutCapacitance(&_n, r, l,g);
            //Assign output node delay
            r.node_delay=get_cell_time(_n.cell_type,r.input_transition_time,0,l);
        }
        default:{
            //Previous transition time
            getAssignInputTransition(&_n, r, l,g);
            //Capacitance from fan-out nodes
            getAssignOutCapacitance(&_n, r, l,g);
            //Assign Cell Delay
            r.node_delay=get_cell_time(_n.cell_type,r.input_transition_time,r.output_cap,l);
        }
        }   //End of the switch statement
        //Increment the total path delay
        path_delay+= r.node_delay;
        Identitfy_violation( analysis, analysis_path.pathtype, path_delay,violate, g);

        print_node_report(outs, &_n, r, path_delay);
    }//End of path analysis

    print_path_report_footer(outs, path_delay);

    if (path_delay > critical_delay && (analysis_path.pathtype==RR || analysis_path.pathtype==IR))
    {
        critical_delay = path_delay;
        critical_path = analysis_path;
        critical_analysis = analysis;
    }

    return path_delay;
}

void analyzePrintPathReports( liberty::Library &l, DAG &g, vector<path>& all_paths, ostream& outs, ofstream& outs2){
    // does path analysis and print report for all paths one by one
    for (path path:all_paths){
        analyzePaths(l, g, path, outs,outs2);
    }
}

//TODO: Implement this
pair <delay_t, path> getCriticalPath( DAG &g,ostream& outs)
{
    outs << "---------------------------------------------------------------------------" << endl;
    outs << "-----------------------------Critical Path --------------------------------------" << endl;
    outs << "---------------------------------------------------------------------------" << endl;
    outs.width(20);
    outs << left;
    outs << fixed;
    outs << "Pin";
    outs.width(20);
    outs << "Cell Type";
    outs.width(20);
    outs << "Type";
    outs.width(20);
    outs << "Increment";
    outs.width(20);
    outs << "Path Delay" << endl;
    outs << "---------------------------------------------------------------------------" << endl;

    delay_t path_delay = 0;
    if(critical_analysis.size()>0){
    analysis_node_t& rIn = (critical_analysis.find(critical_path.start))->second;
    path_delay+= rIn.node_delay;
    node& _n = *g.getNodeByName(critical_path.start);
    print_node_report(outs, &_n, rIn, path_delay);

    for(string _s:critical_path.flow)
    {
        analysis_node_t& r = (critical_analysis.find(_s))->second;
        path_delay+= r.node_delay;
        node& _n = *g.getNodeByName(_s);
        print_node_report(outs, &_n, r, path_delay);
    }
    outs << "---------------------------------------------------------------------------" << endl;
    outs << "Maximum Data Arrival Time \t\t\t\t\t\t\t\t\t" << critical_delay << endl;
    outs << "---------------------------------------------------------------------------" << endl;
}
    return {critical_delay,critical_path};
}

void print_Cell (ostream& outs, node* node_report_ptr, analysis_node_t& r, delay_t path_delay_so_far)
{

    string cellT=node_report_ptr->cell_type;
    string celln= node_report_ptr->name;
    string inpinname,outpinname;
    for( compBox &c: vecComp){
        if(c.type==cellT &&   celln ==c.name  )
        {
            for(pin &ipin:c.inputs)
            {
                if(ipin.pinConn==inn || ipin.pinName==inn)
                {inpinname=ipin.pinName; break;}
            }
            outpinname=c.outputs[0].pinName;
            outn=c.outputs[0].pinConn;
            inn=c.outputs[0].pinConn;
        }
    }
    outs.width(20);
    outs << left;
    outs << fixed;
    string temp;
    temp=node_report_ptr->name +"/"+inpinname;
    outs << temp;//node_report_ptr->name<< "/"<<inpinname;


    outs.width(20);
    outs << node_report_ptr->cell_type;
    outs.width(20);
    outs << NODE_T_NAMES[node_report_ptr->type];

    outs.width(20);
    outs << r.node_delay;
    outs.width(20);
    outs << path_delay_so_far << endl;

    outs.width(20);
    outs << left;
    outs << fixed;
    temp=node_report_ptr->name +"/Y";
    outs << temp;
    //outs << node_report_ptr->name<< "/Y";

    outs.width(20);
    outs << node_report_ptr->cell_type;
    outs.width(20);
    outs << NODE_T_NAMES[node_report_ptr->type];
    outs.width(20);
    outs << r.node_delay;
    outs.width(20);
    outs << path_delay_so_far << endl;

}


//TODO: Implement this
/*
void Identitfy_violation(std::vector<path> paths) {
//enum PATH_T {NA = -1, IR, RR, RO, IO};
for(path p: paths.size())
{
    if(p.pathtype==PATH_T::IR)
    {
        //using update path vector which has delays calculated for each node !!
        //Setup:
       // T ≥ Tpd + InputDelay + OutputDelay − Tskew
         //Hold:
        // Tskew +Thold < InputDelay + Tpd
    }
    else if(p.pathtype==PATH_T::RR){
       // Setup:
        //T≥Tpd +Tcq +Tsetup−Tskew
        //Hold:
       // Tskew +Thold < Tcq + Tpd

    }
    else if (p.pathtype==PATH_T::IO)
    {
        //Setup:
        //T ≥ Tpd + InputDelay + OutputDelay − Tskew

    }
    else  //RO
    {
       // Setup:
         //   T ≥ Tpd + InputDelay + OutputDelay − Tskew
        //Hold:
          //  Tskew +Thold < InputDelay+ Tpd

    }

}
}
*/

void Identitfy_violation( path_analysis_t analysis, PATH_T pathtype, delay_t path_delay,ofstream & violate,DAG &g) {
    //ofstream

    //enum PATH_T {NA = -1, IR, RR, RO, IO};
    delay_t clkPr= g.constraints_map["clk"];
    delay_t skew= g.constraints_map["skew"];
    delay_t setup= g.constraints_map["setup"];
    delay_t hold= g.constraints_map["hold"];
    delay_t tcq= g.constraints_map["tcq"];


    bool setupViolated=false;
    bool holdViolated=false;


    //for(path_analysis_t p: analysis.size())
    //{
    // setupViolated=false;
    //holdViolated=false;

    if(pathtype==PATH_T::IR)
    {
        //using update path vector which has delays calculated for each node !!
        //Setup:
        if(clkPr< path_delay-skew)
        {setupViolated=true;
            violate<<"------------------------------------------\n";
            violate<<"\t \t \t Setup Violated\n" ;
            violate<<"------------------------------------------\n"<<"\n";
        }
        // T ≥ Tpd + InputDelay + OutputDelay − Tskew
        //Hold:
        // Tskew +Thold < InputDelay + Tpd
        if( (skew+hold)>= path_delay)
        { holdViolated=true;
            violate<<"------------------------------------------\n";
            violate<<"\t \t \t Hold Violated\n" ;
            violate<<"------------------------------------------\n";
        }
        if(setupViolated || holdViolated)
        {
            ///////////////////////////////////////////////// pass g
            violate<<"\t \t \t IR path \n";

            violate.width(15);
            violate << left;
            violate<<"node name";
            violate.width(15);
            violate << left;
            violate<<"cell_type \n";
            for(pair<string,analysis_node_t> p: analysis)
            {
                node& _n = *g.getNodeByName(p.first);
                violate.width(15);
                violate << left;
                violate<<p.first;
                violate.width(15);
                violate << left;
                violate<<_n.cell_type<<"\n";
            }

        }

    }
    else{
        if(pathtype==PATH_T::RR)
        {
            // Setup:
            //T≥Tpd +Tcq +Tsetup−Tskew
            if(clkPr< path_delay+tcq+setup-skew)
            {  setupViolated=true;
                violate<<"------------------------------------------\n";
                violate<<"\t \t \t Setup Violated \n" ;
                violate<<"------------------------------------------\n";
            }
            //Hold:
            // Tskew +Thold < Tcq + Tpd                          ///////////cd ???
            if( (skew+hold)>= tcq+path_delay)
            { holdViolated=true;
                violate<<"------------------------------------------\n";
                violate<<"\t \t \t Hold Violated \n" ;
                violate<<"------------------------------------------\n";
            }

            if(setupViolated || holdViolated)
            {
                ///////////////////////////////////////////////// pass g
                violate<<"\t \t \t RR path \n";

                violate.width(15);
                violate << left;
                violate<<"node name";
                violate.width(15);
                violate << left;
                violate<<"cell_type \n";
                for(pair<string,analysis_node_t> p: analysis)
                {
                    node& _n = *g.getNodeByName(p.first);
                    violate.width(15);
                    violate << left;
                    violate<<p.first;
                    violate.width(15);
                    violate << left;
                    violate<<_n.cell_type<<"\n";
                }

            }
        }

    }
}


delay_t getDelayConstraint( string& node_name, DAG& g){
    auto it = g.delay_map.find(node_name);                    //Let's pray the compiler would get it
    if(it != g.delay_map.end()){
        return (*it).second;
    }
    else{
//        cerr  << "Couldnot retrieve delay from delay constraint map: " << node_name << endl;
//        exit(EXIT_FAILURE);
        return 0;
    }
}


cap_t getAssignInputTransition(node* in_node, analysis_node_t& r, Library &l, DAG& g){
    //Iterate over all the previous nodes
    //Compute their transition time
    //Return the maximum
    for(string in_node_name: in_node->in_nodes){                                      //Over all in_node names
        node* node_ref = g.getNodeByName(in_node_name);                     //Get a reference to this node
        //Compute its transition time
        //TODO: Make sure this is working!
        delay_t transition_time = get_transtion_time(node_ref->cell_type,r.input_transition_time,r.output_cap,l);
        r.input_transition_time_list.push_back(transition_time);
    }
    //Find the maximum element
    auto it = max_element(r.input_transition_time_list.begin(),r.input_transition_time_list.end());
    if(it != r.input_transition_time_list.end()){
        return r.input_transition_time = *it;
    }
    else{
        cerr << "Warning: Calling the transition counting on an input node\n";
        return 0;
    }

}

cap_t getAssignOutCapacitance(node* in_node, analysis_node_t& r,  Library &l, DAG &g){
    r.output_cap = 0;
    for(edge _e:in_node->out_edges){
        node* node_ref = g.getNodeByName(_e.n);
        r.output_cap += (get_input_pin_cap(node_ref->cell_type,l) + _e.net_capacitance);
    }
    return r.output_cap;
}

vector<string> topsort(DAG& g){
  vector<string> toReturn;
  ofstream inter("graph.txt");
  for(node & n: g.nodes)
    for(edge _e:n.out_edges)
      inter << n.name << "\t" << _e.n << endl;
  inter.close();
  string command = "tsort graph.txt > sorted.txt";
  system(command.c_str());
  ifstream sorted_list("sorted.txt");
  string line;
  while(getline(sorted_list, line))
    toReturn.push_back(line);
  sorted_list.close();
  return toReturn;

}

void putAAT(Library &l,DAG& g)
{
    vector<string> sorted_list = topsort(g);
    //Create analysis nodes
    path_analysis_t analysis;
    //Fill the analysis nodes by all node names
    for(string node_name :sorted_list){
        node* node_ref = g.getNodeByName(node_name);
        node& n = *node_ref;
        string _s = n.name;
        analysis.insert(make_pair(_s, analysis_node_t()));
        analysis_node_t& r = (analysis.find(_s))->second;
        r.name = _s;
    }



    //Apply the CPM by iterating over all nodes
    for(string node_name :sorted_list){
        node* node_ref = g.getNodeByName(node_name);
        node& n = *node_ref;
        //Find its analysis node
        string _s = n.name;
        analysis_node_t& r = (analysis.find(_s))->second;
        //If it is an input node
        if(n.type == NODE_T::IN){                                      //If input node
            //Delay from file and Transition is zero ~with a French accent
            r.input_transition_time = 0;
        }
        else if(n.type ==NODE_T::OUT){
            //Previous transition time
            getAssignInputTransition(&n, r, l,g);
            //Capacitance from fan-out nodes
            getAssignOutCapacitance(&n, r, l,g);
        }
        else{
            //Previous transition time
            getAssignInputTransition(&n, r, l,g);
            //Capacitance from fan-out nodes
            getAssignOutCapacitance(&n, r, l,g);
        }
    }


    //Apply the CPM by iterating over all nodes
    for(string node_name :sorted_list){
        node* node_ref = g.getNodeByName(node_name);
        node& n = *node_ref;
        //Find its analysis node
        string _s = n.name;
        analysis_node_t& r = (analysis.find(_s))->second;
        //Maximum in-delay from pervious nodes
        delay_t max_delay = 0;
        for (string in: n.in_nodes)
        {
            delay_t delta_in_n;
            node* node_ref = g.getNodeByName(in);
            delta_in_n = node_ref->AAT;
            max_delay= max(max_delay, delta_in_n);
        }
        //Initialize the AAT
        n.AAT = max_delay;
        //If it is an input node
        if(n.type == NODE_T::IN){                                      //If input node
            //Delay from file and Transition is zero ~with a French accent
            n.AAT = max_delay + getDelayConstraint(n.name,g);                     //Increment with my node delay
        }
        else if(n.type ==NODE_T::OUT){
            n.AAT = max_delay + get_cell_time(n.cell_type,r.input_transition_time,0,l);
        }
        else{
            //Assign Cell Delay
            n.AAT = max_delay +  get_cell_time(n.cell_type,r.input_transition_time,r.output_cap,l);
        }
    }
}

void putRAT(Library &l,DAG& g)
{
    vector<string> sorted_list = topsort(g);

    //Create analysis nodes
    path_analysis_t analysis;
    //Fill the analysis nodes by all node names
    for(string node_name :sorted_list){
        node* node_ref = g.getNodeByName(node_name);
        node& n = *node_ref;
        string _s = n.name;
        analysis.insert(make_pair(_s, analysis_node_t()));
        analysis_node_t& r = (analysis.find(_s))->second;
        r.name = _s;
    }



    //Apply the CPM by iterating over all nodes
    for(string node_name :sorted_list){
        node* node_ref = g.getNodeByName(node_name);
        node& n = *node_ref;
        //Find its analysis node
        string _s = n.name;
        analysis_node_t& r = (analysis.find(_s))->second;
        //If it is an input node
        if(n.type == NODE_T::IN){                                      //If input node
            //Delay from file and Transition is zero ~with a French accent
            r.input_transition_time = 0;
        }
        else if(n.type ==NODE_T::OUT){
            //Previous transition time
            getAssignInputTransition(&n, r, l,g);
            //Capacitance from fan-out nodes
            getAssignOutCapacitance(&n, r, l,g);
        }
        else{
            //Previous transition time
            getAssignInputTransition(&n, r, l,g);
            //Capacitance from fan-out nodes
            getAssignOutCapacitance(&n, r, l,g);
        }
    }



    for(auto it = sorted_list.rbegin(); it!= sorted_list.rend(); it++){
      string node_name = *it;
      node* node_ref = g.getNodeByName(node_name);
      node& n = *node_ref;
      //Find its analysis node
      string _s = n.name;
      if(n.out_edges.empty()){
        n.RAT = g.constraints_map["clk"];
      }
      else if(node_name == "START"){
        n.RAT = 0;
        continue;
      }
      else{
        delay_t min_rat = std::numeric_limits<float>::max();
        node* min_rat_node;
        for(edge _e:n.out_edges){
          delay_t temp;
          node* out_ref = g.getNodeByName(_e.n);
          temp = out_ref->RAT;
          if(temp < min_rat){
            min_rat_node = out_ref;
          }
          min_rat = min(min_rat, temp);
        }
      string _s_out = min_rat_node->name;
      analysis_node_t& r_o = (analysis.find(_s_out))->second;
      delay_t to_subtract =
      get_cell_time(min_rat_node->cell_type,r_o.input_transition_time,r_o.output_cap,l);

      n.RAT = min_rat - to_subtract;

      }
    }

}

void CPM(Library &l,DAG& g)
{
    putAAT(l,g);
    putRAT(l,g);
    for (node& n :g.nodes)
    {
        n.slack = n.RAT - n.AAT;
    }
}
