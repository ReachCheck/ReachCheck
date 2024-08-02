//
// Created by vm on 9/3/21.
//

#include "BasicMethod.h"


//计算所有节点的直接后继
void allnodes_next(Graph *g, vector<vector<unsigned> > &next){
    for (unsigned i = 0; i < g->num_nodes(); i++) {
        for (unsigned j = 0; j < g->get_neighbors(i)->size(); j++) {
            next[i].push_back((*(g->get_neighbors(i)))[j]);
        }
    }
}

//计算所有节点的直接前驱
void allnodes_prior(Graph *g, vector<vector<unsigned> > &prior){
    for (unsigned i = 0; i < g->num_nodes(); i++) {
        for (unsigned j = 0; j < g->get_predecessors(i)->size(); j++) {
            prior[i].push_back((*(g->get_predecessors(i)))[j]);
        }
    }
}

//计算一个节点的out和in，out用来存后代，即后继，in用来存祖先，即前驱
void onenode_out_in(vector<unsigned> &onenode_out, vector<unsigned> &onenode_in, unsigned node,
                    vector<vector<unsigned> > next, vector<vector<unsigned> > prior) {
    stack<unsigned> s;
    //计算out
    s.push(node);
    while (!s.empty()) {
        int temp = s.top();
        if (!count(onenode_out.begin(), onenode_out.end(), temp)) {
            onenode_out.push_back(temp);
        }
        s.pop();
        //如果当前节点有直接后继，将直接后继挨个入栈，并将直接后继从temp_out中删除
        while (!next[temp].empty()) {
            s.push(next[temp].back());
            next[temp].pop_back();
        }
    }
    sort(onenode_out.begin(), onenode_out.end());

    //计算in
    s.push(node);
    while (!s.empty()) {
        int temp = s.top();
        if (!count(onenode_in.begin(), onenode_in.end(), temp)) {
            onenode_in.push_back(temp);
        }
        s.pop();
        //如果当前节点有直接前驱，将直接前驱挨个入栈，并将直接前驱从temp_in中删除
        while (!prior[temp].empty()) {
            s.push(prior[temp].back());
            prior[temp].pop_back();
        }
    }
    sort(onenode_in.begin(), onenode_in.end());
}

//计算所有节点的out和in，out用来存后代，即后继，in用来存祖先，即前驱
void construct_out_in(vector<vector<unsigned> > &out, vector<vector<unsigned> > &in,
                      Graph *g, vector<vector<unsigned> > next, vector<vector<unsigned> > prior) {
    vector<unsigned> onenode_out;
    vector<unsigned> onenode_in;

    for (unsigned i = 0; i < g->num_nodes(); i++) {
        onenode_out_in(onenode_out, onenode_in, i, next, prior);
        out.push_back(onenode_out);
        in.push_back(onenode_in);
        onenode_out.clear();
        onenode_in.clear();
    }
}

//计算拓扑排序
void topological_sort(vector<unsigned> &topo_order, Graph *g) {
    vector<unsigned> node_indegree;
    node_indegree = vector<unsigned>(g->num_nodes(), 0);

    for (unsigned i = 0; i < g->num_nodes(); i++) {
        node_indegree[i] = g->get_predecessors(i)->size();
    }

    stack<unsigned> s;

    for (unsigned i = 0; i < g->num_nodes(); i++) {
        if (node_indegree[i] == 0) {
            s.push(i);
        }
    }
    while (!s.empty()) {
        int temp = s.top();
        topo_order.push_back(temp);
        s.pop();
        for (unsigned i = 0; i < g->get_neighbors(temp)->size(); i++) {
            int temp2 = (*g->get_neighbors(temp))[i];
            node_indegree[temp2]--;
            if (node_indegree[temp2] == 0) {
                s.push(temp2);
            }
        }
    }
}

//计算逆拓扑排序
void reverse_topological_sort(vector<unsigned> &reverse_topo_order, Graph *g) {
    vector<unsigned> node_degree;
    node_degree = vector<unsigned>(g->num_nodes(), 0);

    for (unsigned i = 0; i < g->num_nodes(); i++) {
        node_degree[i] = g->get_neighbors(i)->size();
    }

    stack<unsigned> s;

    for (unsigned i = 0; i < g->num_nodes(); i++) {
        if (node_degree[i] == 0) {
            s.push(i);
        }
    }
    while (!s.empty()) {
        int temp = s.top();
        reverse_topo_order.push_back(temp);
        s.pop();
        for (unsigned i = 0; i < g->get_predecessors(temp)->size(); i++) {
            int temp2 = (*g->get_predecessors(temp))[i];
            node_degree[temp2]--;
            if (node_degree[temp2] == 0) {
                s.push(temp2);
            }
        }
    }
}

//计算独立排列
void compute_permutation(vector<unsigned> &permutation, Graph *g) {
    for (unsigned i = 0; i < g->num_nodes(); i++) {
        permutation.push_back(i);
    }
    srand((unsigned) time(0));
    for (unsigned i = g->num_nodes() - 1; i > 0; i--) {
        unsigned j = rand() % i;
        swap(permutation[i], permutation[j]);
    }
}
