/*
 * Graph.h
 *
 *  Created on: 2021年7月8日
 *      Author: vm
 */

#ifndef SRC_GRAPH_H_
#define SRC_GRAPH_H_

#include "Basic.h"

class Graph {
private:
	unsigned n, m;	//n个结点，m条边
	vector<vector<unsigned> > nb;	//直接后继
	vector<vector<unsigned> > pd;	//直接前驱
	vector<unsigned> deg;			//出度
	vector<unsigned> indeg;			//入度
	deque<unsigned> leaves;			//叶子结点
	vector<unsigned> roots;			//根结点

public:
	Graph(const string &filename);
	~Graph();

	//通过把类成员函数声明为const 以表明它们不修改类对象。
	const vector<unsigned>* get_neighbors(unsigned node) const;

	vector<unsigned>* get_roots();	//得到根结点


	inline deque<unsigned>* get_leaves() {	//inline的函数不能赋值给新的变量，只能用对象直接使用
		return &leaves;
	}

	inline vector<unsigned>* get_degrees() {
		return &deg;
	}

	inline vector<unsigned>* get_indegrees() {
		return &indeg;
	}

	const inline vector<unsigned>* get_predecessors(unsigned v) const {
		return &pd[v];
	}

	const inline vector<vector<unsigned> > get_prior() const {
		return pd;
	}
	const inline vector<vector<unsigned> > get_next() const {
		return nb;
	}

    void setNext(vector<vector<unsigned> > nb){
        this->nb = nb;
    }

    void setPrior(vector<vector<unsigned> > pd){
        this->pd = pd;
    }

	void setRoots(vector<unsigned> roots){
		this->roots = roots;
	}

	void setLeaves(deque<unsigned> leaves){
		this->leaves = leaves;
	}

	inline unsigned num_nodes() const {
		return n;
	}
};

#endif /* SRC_GRAPH_H_ */
