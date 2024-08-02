//
// Created by vm on 9/3/21.
//

#ifndef IP_BASICMETHOD_H
#define IP_BASICMETHOD_H

#include "Basic.h"
#include "Graph.h"
#include "genIndexMethods.h"

void allnodes_next(Graph *g, vector<vector<unsigned> > &next);

void allnodes_prior(Graph *g, vector<vector<unsigned> > &prior);

void onenode_out_in(vector<unsigned> &onenode_out, vector<unsigned> &onenode_in, unsigned node,
                    vector<vector<unsigned> > next, vector<vector<unsigned> > prior);

void construct_out_in(vector<vector<unsigned> > &out, vector<vector<unsigned> > &in,
                      Graph *g, vector<vector<unsigned> > next, vector<vector<unsigned> > prior);

void topological_sort(vector<unsigned> &topo_order, Graph *g);

void reverse_topological_sort(vector<unsigned> &reverse_topo_order, Graph *g);

void compute_permutation(vector<unsigned> &permutation, Graph *g);

#endif //IP_BASICMETHOD_H
