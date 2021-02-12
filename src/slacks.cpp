#include "cpm.h"
#include "netlist.h"
#include "DAG.h"
#include <fstream>

using namespace std;

bool assert_stream_working(ofstream& st, const string& stream_name = "unknown stream name"){
  if(st.fail() || !st.is_open()){
    cerr << "Stream Failed: " << stream_name << endl;
    exit(EXIT_FAILURE);
  }
}

bool assert_stream_working(ifstream& st, const string& stream_name = "unknown stream name"){
  if(st.fail() || !st.is_open()){
    cerr << "Stream Failed: " << stream_name << endl;
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char** argv){
  //Parse the Libert File
  Library l = parse(argv[1]);
  DAG g;
  //delay_t clk_time, setup_time, hold_time, skew_time;

  //Open Input File Streams
  ifstream netlist_stream(argv[2]);
  ifstream cap_stream(argv[3]);
  ifstream constraint_stream(argv[4]);
  ifstream clk_stream(argv[5]);

  //Open Output File Streams
  ofstream graph_stream(argv[6]);

  //Assert all streams are successful
  assert_stream_working(netlist_stream, argv[2]);
  assert_stream_working(cap_stream, argv[3]);
  assert_stream_working(constraint_stream,argv[4]);
  assert_stream_working(clk_stream,argv[5]);
  assert_stream_working(graph_stream,argv[6]);


  parse_netlist(netlist_stream, cap_stream,constraint_stream, clk_stream, g);
  CPM(l,g);
  g.outputTiming( graph_stream);

  //Close all streams
  netlist_stream.close();
  cap_stream.close();
  constraint_stream.close();
  clk_stream.close();
  graph_stream.close();
}
