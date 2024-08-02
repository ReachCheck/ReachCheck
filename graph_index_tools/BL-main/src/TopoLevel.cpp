//
// Created by vm on 9/3/21.
//

#include "TopoLevel.h"


//计算一个节点的拓扑level
void onenode_topolevel(Graph *g, unsigned node, vector<unsigned> &topo_level)
{
    if ((*(g->get_indegrees()))[node]==0)
    {
        topo_level[node] = 0;
    }
    else
    {
        topo_level[node] = 1;
        const vector <unsigned> *p;
        p=g->get_predecessors(node);
        for (unsigned j = 0; j < g->get_predecessors(node)->size(); j++) {
            topo_level[node] = max(topo_level[node], 1 + topo_level[(*p)[j]]);
        }
    }
}

/*//按照拓扑排序计算所有节点的拓扑level
void allnodes_topolevel(Graph *g, vector<unsigned> topo_order, vector<unsigned> &topo_level,
                        unsigned &max_level)
{
    topo_level.clear();
    topo_level.resize(g->num_nodes());
    max_level = 0;
    for (unsigned i = 0; i < topo_order.size(); i++)
    {
        onenode_topolevel(g, topo_order[i], topo_level);
        if(topo_level[topo_order[i]] > max_level)
        {
            max_level = topo_level[topo_order[i]];
        }
    }
}*/

//按照拓扑排序计算所有节点的拓扑level
void allnodes_topolevel(Graph *g, vector<unsigned> topo_order, vector<unsigned> &topo_level)
{
    topo_level.clear();
    topo_level.resize(g->num_nodes());
    cout<<"topo_order.size:"<<topo_order.size()<<endl;
    for (unsigned i = 0; i < topo_order.size(); i++)
    {
        onenode_topolevel(g, topo_order[i], topo_level);
    }
}

//计算一个节点的逆拓扑level
void onenode_reverse_topolevel(Graph *g, unsigned node, vector<unsigned> &reverse_topo_level) {
    if ((*(g->get_degrees()))[node] == 0) {
        reverse_topo_level[node] = 0;
    } else {
        reverse_topo_level[node] = 1;
        for (unsigned j = 0; j < g->get_neighbors(node)->size(); j++) {
            reverse_topo_level[node] = max(reverse_topo_level[node], 1 + reverse_topo_level[(*(g->get_neighbors(node)))[j]]);
        }
    }
}

//按照逆拓扑排序计算所有节点的逆拓扑level
void allnodes_reverse_topolevel(Graph *g, vector<unsigned> reverse_topo_order,
                                vector<unsigned> &reverse_topo_level){
    reverse_topo_level.clear();
    reverse_topo_level.resize(g->num_nodes());
    for (unsigned i = 0; i < reverse_topo_order.size(); i++) {
        onenode_reverse_topolevel(g, reverse_topo_order[i], reverse_topo_level);
    }
}

//通过拓扑level进行过滤
bool topolevel_filter(Graph *g, vector<unsigned> topo_level, unsigned u, unsigned v){
    if(u > g->num_nodes() - 1 || v > g->num_nodes() - 1){ //u，v不在图中，直接返回false
        return false;
    }
    if(u == v){
        return true;
    }else{
        if(topo_level[u] >= topo_level[v]){
            return false;
        }
    }
    return true;
}

//将节点u的后代分层
void compute_level_descendant(unsigned node, vector<vector<unsigned> > out, unsigned max_level,
                              vector<unsigned> topo_level, vector<vector<unsigned> > &descendant_level){
    descendant_level = vector<vector<unsigned> >(max_level + 1);
    for(unsigned i = 0; i < out[node].size(); i++){
        for(unsigned j = 0; j <= max_level; j++){
            if(topo_level[out[node][i]] == j){
                descendant_level[j].push_back(out[node][i]);
                break;
            }
        }
    }
}

//节点u的后代每一层有多少个
void compute_levelnums_descendant(unsigned node, vector<vector<unsigned> > descendant_level,
                                  vector<vector<unsigned> > &descendant_levelnums, vector<unsigned> &all_descendant_level,
                                  vector<unsigned> &max_descendant_levelnums){
    unsigned count_all_descendant_level = 0;
    unsigned count_max_descendant_levelnums = 0;
    unsigned count_max_descendant_level = 0;
    for (unsigned i = 0; i < descendant_level.size(); i++) {
        unsigned count_descendant_levelnums = 0;
        for (unsigned j = 0; j < descendant_level[i].size(); j++) {
            count_descendant_levelnums++;
        }
        descendant_levelnums[node].push_back(count_descendant_levelnums);
        if(count_max_descendant_levelnums < count_descendant_levelnums){
            count_max_descendant_levelnums = count_descendant_levelnums;
        }
        if(count_descendant_levelnums != 0){
            ++count_all_descendant_level;
        }
    }
    all_descendant_level.push_back(count_all_descendant_level);
    max_descendant_levelnums.push_back(count_max_descendant_levelnums);
}

//将节点u的祖先分层
void compute_level_pre(unsigned node, vector<vector<unsigned> > in, unsigned max_level,
                       vector<unsigned> topo_level, vector<vector<unsigned> > &pre_level){
    pre_level = vector<vector<unsigned> >(max_level + 1);
    for(unsigned i = 0; i < in[node].size(); i++){
        for(unsigned j = 0; j <= max_level; j++){
            if(topo_level[in[node][i]] == j){
                pre_level[j].push_back(in[node][i]);
                break;
            }
        }
    }
}

//节点u的祖先每一层有多少个
void compute_levelnums_pre(unsigned node, vector<vector<unsigned> > pre_level,
                           vector<vector<unsigned> > &pre_levelnums, vector<unsigned> &all_pre_level,
                           vector<unsigned> &max_pre_levelnums){
    unsigned count_all_pre_level = 0;
    unsigned count_max_pre_levelnums = 0;
    for (unsigned i = 0; i < pre_level.size(); i++) {
        unsigned count_pre_levelnums = 0;
        for (unsigned j = 0; j < pre_level[i].size(); j++) {
            count_pre_levelnums++;
        }
        pre_levelnums[node].push_back(count_pre_levelnums);
        if(count_max_pre_levelnums < count_pre_levelnums){
            count_max_pre_levelnums = count_pre_levelnums;
        }
        if(count_pre_levelnums != 0){
            ++count_all_pre_level;
        }
    }
    all_pre_level.push_back(count_all_pre_level);
    max_pre_levelnums.push_back(count_max_pre_levelnums);
}

//利用节点的祖先的每一层个数和后代的每一层个数进行过滤
bool descendant_levelnums_filter(unsigned u, unsigned v, vector<unsigned> max_descendant_level,
                                 vector<unsigned> all_descendant_level){
    if(all_descendant_level[u] <= all_descendant_level[v]){
        return false;
    }
    if(max_descendant_level[u] < max_descendant_level[v]){
        return false;
    }
    return true;
}

bool pre_levelnums_filter(unsigned u, unsigned v, vector<unsigned> max_pre_level, vector<unsigned> all_pre_level){
    if(all_pre_level[v] <= all_pre_level[u]){
        return false;
    }
    if(max_pre_level[v] < max_pre_level[u]){
        return false;
    }
    return true;
}
