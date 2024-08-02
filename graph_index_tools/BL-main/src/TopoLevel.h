//
// Created by vm on 9/3/21.
//

#ifndef IP_TOPOLEVEL_H
#define IP_TOPOLEVEL_H

#include "Basic.h"
#include "Graph.h"

void onenode_topolevel(Graph *g, unsigned node, vector<unsigned> &topo_level);

void allnodes_topolevel(Graph *g, vector<unsigned> topo_order, vector<unsigned> &topo_level);

void onenode_reverse_topolevel(Graph *g, unsigned node, vector<unsigned> &reverse_topo_level);

void allnodes_reverse_topolevel(Graph *g, vector<unsigned> reverse_topo_order,
                                vector<unsigned> &reverse_topo_level);

bool topolevel_filter(Graph *g, vector<unsigned> topo_level, unsigned u, unsigned v);

//unsigned maxlevel(vector<unsigned> topo_level);

void compute_level_descendant(unsigned node, vector<vector<unsigned> > out, unsigned max_level,
                              vector<unsigned> topo_level, vector<vector<unsigned> > &descendant_level);

void compute_levelnums_descendant(unsigned node, vector<vector<unsigned> > descendant_level,
                                  vector<vector<unsigned> > &descendant_levelnums, vector<unsigned> &all_descendant_level,
                                  vector<unsigned> &max_descendant_levelnums);

void compute_level_pre(unsigned node, vector<vector<unsigned> > out, unsigned max_level,
                       vector<unsigned> topo_level, vector<vector<unsigned> > &pre_level);

void compute_levelnums_pre(unsigned node, vector<vector<unsigned> > pre_level,
                           vector<vector<unsigned> > &pre_levelnums, vector<unsigned> &all_pre_level,
                           vector<unsigned> &max_pre_levelnums);

bool descendant_levelnums_filter(unsigned u, unsigned v, vector<unsigned> max_descendant_level,
                                 vector<unsigned> all_descendant_level);

bool pre_levelnums_filter(unsigned u, unsigned v, vector<unsigned> max_pre_level, vector<unsigned> all_pre_level);


#endif //IP_TOPOLEVEL_H
