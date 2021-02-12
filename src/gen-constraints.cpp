#include "gen-constraints.h"

float rand_float(float LO, float HI){
    //From: http://stackoverflow.com/questions/686353/c-random-float-number-generation
    return LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
}


void write_random_net_cap(const vector<string>& net_names, ostream& outs){
    for(string net_name : net_names){  //For any net name
        outs << net_name << "\t" << setprecision(3) << rand_float(MIN_NET_CAP, MAX_NET_CAP) << "\n";
    }
}

void write_random_timing_constraints(const vector<string>& input_names, ostream& outs){
    for(string input_name : input_names){  //For any input name
        outs << input_name << "\t" << setprecision(3) << rand_float(MIN_INPUT_TIMING, MAX_INPUT_TIMING) << "\n";
    }
}

void parse_net_input(vector<string>& input_names, vector<string>& wire_names, ifstream& netlist_stream){
    regex port_pattern("\\s*(input|wire)\\s*(.+)\\s*");
    regex port_array_pattern("\\s*(input|wire)\\s*\\[([0-9]+):([0-9]+)\\]\\s*([^\\s]*)\\s*");
    string line;
    while(getline(netlist_stream, line, ';')){
        if(regex_match(line, port_array_pattern)){
            smatch sm;
            regex_search(line, sm, port_array_pattern);
            //Assign the matching
            string port_type    = sm[1];
            int end             = stoi(sm[2]);
            int start           = stoi(sm[3]);
            string port_name    = sm[4];
            vector<string>& matching_vector = (port_type == "wire")?wire_names : input_names;
            for(int ii = start; ii <= end; ii++)
              matching_vector.push_back(port_name + string("[") + to_string(ii) + string("]"));   //Just write my_wire[ii]
        }
        else if(regex_match(line, port_pattern)){
            smatch sm;
            regex_search(line, sm, port_pattern);
            string type = sm[1];
            string name = sm[2];
            vector<string>& matching_vector = (type == "wire")?wire_names : input_names;
            matching_vector.push_back(name);
        }
    }
}

/**
 *  Clk Period
 *  Clk Skew
 *  Input delay for each input port
 *  For each wire, capacitance
 */
int main(int argc, char** argv){
    srand(42);                              //For replication
    ifstream netlist_stream(argv[1]);
    if(!netlist_stream.is_open() || netlist_stream.fail()){
        cerr << "Input stream failed" << endl;
        exit(EXIT_FAILURE);
    }
    ofstream cap_stream(argv[2]);
    ofstream constraint_stream(argv[3]);

    vector<string> input_names, wire_names;

    parse_net_input(input_names, wire_names, netlist_stream);
    write_random_net_cap(wire_names, cap_stream);
    write_random_timing_constraints(input_names, constraint_stream);

    //Close streams
    netlist_stream.close();
    cap_stream.close();
    constraint_stream.close();
    return EXIT_SUCCESS;
}
