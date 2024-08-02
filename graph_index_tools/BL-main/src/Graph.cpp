/*
 * Graph.cpp
 *
 *  Created on: 2021年7月8日
 *      Author: vm
 */
#include "Graph.h"

Graph::Graph(const string &filename) :
		n(0), m(0) {
	string line;
	ifstream stream(filename.c_str());	//c_str() 以 char* 形式传回 string 内含字符串
	if (!stream.eof()) {	//eof（）返回true时是读到文件结束符0xFF，而文件结束符是最后一个字符的下一个字符。
		getline(stream, line);	//读取每行的数据
		istringstream iss(line, istringstream::in);	//用空格作为字符串分隔符，赋值n和m
		iss >> n >> m;
		nb = vector<vector<unsigned> >(n);	//n行
		pd = vector<vector<unsigned> >(n);	//n行

		deg = vector<unsigned>(n, 0);		//n个元素,初值为0,出度
		indeg = vector<unsigned>(n,0);		//入度
		leaves = deque<unsigned>();
		unsigned s, t;
		vector<bool> is_root(n, true);
		getline(stream, line);

		while (stream.good()) {				//用来判断当前流的状态(读写正常(即符合读取和写入的类型)
			iss.clear();
			iss.str(line);					//把line中的字符串存入字符串流中
			iss >> s;						//源结点
			while (iss >> t) {				//t是目的结点
				nb[s].push_back(t);			//向量尾部增加一个元素X
				pd[t].push_back(s);
				++deg[s];                   //出度
				++indeg[t];					//入度
				is_root[t] = false;
			}
			assert(s < n);
			getline(stream, line);
		}
		for(unsigned i=0;i<n;i++){
			if(deg[i]==0){
				//出度为0，叶子
				leaves.push_back(i);
			}
		    if (is_root[i]) {
		      roots.push_back(i);
		    }
		}
	}
}

Graph::~Graph() {
}

const vector<unsigned>* Graph::get_neighbors(unsigned v) const {
  return &nb[v];
}

vector<unsigned>* Graph::get_roots() {
  return &roots;
}
