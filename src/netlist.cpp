#ifndef NETLIST_H
#define NETLIST_H
#include "parser.h"
#include "netlist.h"
#include <string>

using namespace std;

bool isFF(std::string s)
{
    return (s.find("DFF") == 0);
}

void fill_DAG(DAG &g)
{
    for( compBox &c: vecComp)
    {
        if (isFF(c.type))
        {
            //DONE: Add Cell Type Information for both the D and Q pins: Both are the same
            g.nodes.push_back(node("d_"+c.name,FFD,c.type));
            g.nodes.push_back(node("q_"+c.name,FFQ,c.type));

            g.join("clk","clk","q_"+c.name);
        }
        else
        {
            //DONE: Add Cell Type Inormation using the new constructor signature

            g.nodes.push_back(node(c.name,CELL,c.type));
        }
    }
//input connected to componet
    for( node &n: g.nodes)
    {
        if (n.type== IN && n.name != "clk")
        {
            for( compBox &d: vecComp)
            {
                for(pin &ipin:d.inputs)
                {
                    if(ipin.pinConn==n.name)
                    {
                        if (isFF(d.type))
                        {
                            //updated
                            g.join(n.name,n.name,"d_"+d.name);
                        }
                        else
                        {
                             //updated

                            g.join(n.name,n.name,d.name);
                        }
                    }

                }
            }
        }
    }
//component connected to another component

    for( compBox &c: vecComp)
    {
        for(pin &opin:c.outputs )
        {
            for( compBox &d: vecComp)
            {
                for(pin &ipin:d.inputs)
                {
                    if(ipin.pinConn==opin.pinConn)
                    {   //second component is FF
                        if (isFF(c.type))
                        { //first component is FF
                            if (isFF(d.type))
                            {

                                g.join(ipin.pinConn,"q_"+c.name,"d_"+d.name);//updated
                            }
                            else //first component is a gate not FF
                            {

                                g.join(ipin.pinConn,"q_"+c.name,d.name); //updated
                            }
                        }
                        else //second component is a gate not FF
                        {
                            if (isFF(d.type)) //first component is  FF
                            {

                                g.join(ipin.pinConn,c.name,"d_"+d.name); //updated
                            }
                            else //first component is a gate not FF
                            {

                                g.join(ipin.pinConn,c.name,d.name); //updated
                            }
                        }

                    }
                }
            }
            //component connected to output node
            for( node &d: g.nodes)
            {
                if (d.type== OUT)
                {
                    if(opin.pinConn==d.name)
                    {
                        if (isFF(c.type))
                            g.join(opin.pinConn,"q_"+c.name,d.name); //updated
                        else
                            g.join(opin.pinConn,c.name,d.name); //updated
                    }

                }
            }


        }

    }
}


void read_map_list(std::ifstream &caplist, cap_map_t & _map)
{
    string netname;
    cap_t  capValue;
    if(caplist.is_open())
    {
        while(!caplist.eof())
        {
            caplist>>netname;
            caplist>>capValue;
            _map.insert({netname,capValue});
        }
    }
}

//open caplist fstream before passing !!
//close afterwards

void parse_netlist(std::ifstream &netlist, std::ifstream &cap_stream,std::ifstream &constraint_stream, std::ifstream & clk_stream, DAG &g)
{
  read_map_list(cap_stream, g.cap_map);
  read_map_list(constraint_stream, g.delay_map);
  read_map_list(clk_stream, g.constraints_map);
  read_netlist(netlist, vecComp, g);
  fill_DAG(g);
}


#endif // NETLIST_H
