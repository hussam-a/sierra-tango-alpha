#include "path_finding.h"

std::vector<path > get_paths_graph( DAG &g)
{
    std::vector<path> paths;
    for(node &n: g.nodes)
    {
        if ( n.type == OUT || n.type == FFD || n.type == CELL){
            continue;
        }
        std::vector<path> paths_new = get_paths_node(n,g);
        paths.insert( paths.end(), paths_new.begin(), paths_new.end() );
    }
    return paths;
}

std::vector<path> get_paths_node(node &n, DAG &g)
{
    std::vector<path> paths;
    for (const edge &e: n.out_edges)
    {
        path local_path;
        local_path.start = n.name;

        node* node_ref = g.getNodeByName(e.n);
        get_paths_recursive(*node_ref, g, local_path, paths);
    }
    return paths;
}


void get_paths_recursive(node &n, DAG &g, path whole_path, std::vector<path>& all_paths)
{
    NODE_T current_node_type = n.type;
    whole_path.flow.push_back(n.name);
    node* node_ref = g.getNodeByName(whole_path.start);
    PATH_T resolved_path_type = get_path_type(node_ref->name,node_ref->type, current_node_type);

    if(resolved_path_type != -1){
        whole_path.end = n.name;
        whole_path.pathtype = resolved_path_type;
        all_paths.push_back(whole_path);
        return;
    }
    else{
        for (const edge &e: n.out_edges)
        {
            node* node_ref = g.getNodeByName(e.n);
            get_paths_recursive(*node_ref, g, whole_path, all_paths);
        }
    }
}
