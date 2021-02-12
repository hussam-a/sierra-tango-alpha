#include "netlist.h"
#include "path_finding.h"
#include <fstream>
using namespace std;

int main(int argc, char *argv[])
{
    DAG g;
    ifstream netlist;
    ofstream path_file;

    if (argc>1)
    {
        netlist.open(argv[1], std::ifstream::in);
        if(argc > 2){
           path_file.open(argv[2], std::ofstream::out);
        }

    }
    if (netlist.is_open())
    {
        read_netlist(netlist,vecComp, g);        //Do not use parse_netlist; Use read_netlist
        fill_DAG(g);
    }

    vector<path> paths;
    if (netlist.is_open())
    {
        paths = get_paths_graph(g);
    }

    ostream& out = (path_file.is_open())?path_file:cout;

    for (path &p: paths)
    {
    	node* start = g.getNodeByName(p.start);
        out << PATH_NAMES[p.pathtype] << ":\t\t" << p.start << " (" << NODE_T_NAMES[start->type] << ")\t";
        for (const string n_name:p.flow)
        {
          	node* n = g.getNodeByName(n_name);
            out << n->name << " (" << NODE_T_NAMES[n->type] << ")\t";
        }
        out << endl;
    }

    if(netlist.is_open()) netlist.close();
    if(path_file.is_open()) path_file.close();

	return 0;
}
