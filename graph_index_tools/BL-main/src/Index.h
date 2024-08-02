/*
 * Index.h
 *
 *  Created on: 2021年7月9日
 *      Author: vm
 */

#ifndef SRC_INDEX_H_
#define SRC_INDEX_H_

#include "Basic.h"
#include "Graph.h"
#include "BasicMethod.h"
#include "genIndexMethods.h"
#include "TopoLevel.h"


void construct_minV_BL(vector<vector<unsigned> > &Lout, vector<vector<unsigned> > &Lin, Graph *g,
		vector<unsigned> topo_order, vector<unsigned> reverse_topo_order, vector<unsigned> permutation,
						vector<vector<unsigned> > next, vector<vector<unsigned> > prior);

void labelingnums(Graph *g, vector<vector<unsigned> > L_out_TOL, vector<vector<unsigned> > L_in_TOL,
		vector<unsigned> &onenode_labelingnums, unsigned &allnodes_labelingnums);

void topo_fold_degree(unsigned num, unsigned N, unsigned &topoFoldDegree);

void maxPathLen(Graph *g, vector<unsigned> topo_level, vector<unsigned> reverse_topo_level,
				vector<unsigned> &max_pathLen);

bool cmp(tuple<unsigned, unsigned, unsigned> num1, tuple<unsigned, unsigned, unsigned> num2);

void nodesPermutationNum(Graph *g, vector<unsigned> topo_level, vector<unsigned> max_pathLen,
						 vector<tuple<unsigned, unsigned, unsigned> > &order);

void update_graph(unsigned delta,vector<vector<unsigned> > &next,vector<vector<unsigned> > &prior,
				  vector<tuple<unsigned, unsigned, unsigned> > order);

void allNodesOrder(Graph *g,unsigned delta,vector<vector<unsigned> > next,vector<vector<unsigned> > prior,
				   vector<tuple<unsigned, unsigned, unsigned> > order, vector<unsigned> &delta_nodesPermutation);

void construct_minV_BL_k(vector<vector<unsigned> > &Lout_k, vector<vector<unsigned> > &Lin_k,
                         unsigned k, Graph *g, vector<unsigned> topo_order, vector<unsigned> reverse_topo_order,
                         vector<unsigned> permutation, vector<vector<unsigned> > next, vector<vector<unsigned> > prior);

bool is_lessthan_MinV_k(vector<unsigned> arr1, vector<unsigned> arr2);

bool query_minV_BL(unsigned u, unsigned v);


#endif /* SRC_INDEX_H_ */
