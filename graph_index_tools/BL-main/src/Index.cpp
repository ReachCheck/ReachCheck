/*
 * Index.cpp
 *
 *  Created on: 2021年7月9日
 *      Author: vm
 */

#include "Index.h"

void construct_minV_BL(vector<vector<unsigned> > &Lout,
		vector<vector<unsigned> > &Lin,
		Graph *g,
		vector<unsigned> topo_order,
		vector<unsigned> reverse_topo_order,
		vector<unsigned> permutation,
		vector<vector<unsigned> > next,
		vector<vector<unsigned> > prior)
{
    Lout = vector<vector<unsigned> >(g->num_nodes());
    Lin = vector<vector<unsigned> >(g->num_nodes());

    vector<unsigned> tmp_v;

	vector<unsigned> reverse_permutation(g->num_nodes());
	for(unsigned i = 0; i < permutation.size(); ++i) {
		reverse_permutation[permutation[i]] = i;
	}

	//计算Lout
	for (unsigned i = 0; i < reverse_topo_order.size(); i++) {
		for(unsigned j = 0; j < next[reverse_topo_order[i]].size(); ++j){
			unsigned temp = next[reverse_topo_order[i]][j];
			for(unsigned k = 0; k < Lout[temp].size(); ++k){
				if(Lout[temp][k] < permutation[reverse_topo_order[i]]){
                    Lout[reverse_topo_order[i]].push_back(Lout[temp][k]);
				}
			}
			if(permutation[temp] < permutation[reverse_topo_order[i]])
			{
				Lout[reverse_topo_order[i]].push_back(permutation[temp]);
			}
			else
			{
                Lout[reverse_topo_order[i]].push_back(permutation[reverse_topo_order[i]]);
			}
		}
		sort(Lout[reverse_topo_order[i]].rbegin(), Lout[reverse_topo_order[i]].rend());
		tmp_v.clear();

		if(Lout[reverse_topo_order[i]].size()!=0)
		{
			tmp_v.push_back(Lout[reverse_topo_order[i]][0]);
			for(unsigned j=1;j<Lout[reverse_topo_order[i]].size();j++)
			{
				if(Lout[reverse_topo_order[i]][j]<tmp_v[tmp_v.size()-1])
				{
					tmp_v.push_back(Lout[reverse_topo_order[i]][j]);
				}
			}
			Lout[reverse_topo_order[i]].clear();
			Lout[reverse_topo_order[i]]=tmp_v;
		}
	}

	//计算Lin
	for (unsigned i = 0; i < topo_order.size(); i++) {
		for(unsigned j = 0; j < prior[topo_order[i]].size(); ++j){
			unsigned temp = prior[topo_order[i]][j];
			for(unsigned k = 0; k < Lin[temp].size(); ++k){
				if(Lin[temp][k] < permutation[topo_order[i]]){
                    Lin[topo_order[i]].push_back(Lin[temp][k]);
				}
			}
			if(permutation[temp] < permutation[topo_order[i]])
			{
				Lin[topo_order[i]].push_back(permutation[temp]);
			}
			else
			{
                Lin[topo_order[i]].push_back(permutation[topo_order[i]]);
			}
		}
		sort(Lin[topo_order[i]].rbegin(), Lin[topo_order[i]].rend());
		tmp_v.clear();
		if(Lin[topo_order[i]].size()!=0)
		{
			tmp_v.push_back(Lin[topo_order[i]][0]);
			for(unsigned j=1;j<Lin[topo_order[i]].size();j++)
			{
				if(Lin[topo_order[i]][j]<tmp_v[tmp_v.size()-1])
				{
					tmp_v.push_back(Lin[topo_order[i]][j]);
				}
			}
			Lin[topo_order[i]].clear();
			Lin[topo_order[i]]=tmp_v;
		}
	}

	//按降序删除错误节点
	for(unsigned i = 0; i < g->num_nodes(); ++i)
	{
		vector <unsigned> tmp_v;
		tmp_v.clear();
		if(Lout[i].size() != 0){
			for(unsigned j = 0; j < Lout[i].size(); j++) {
				unsigned label = 0;
				int k = j + 1;
				int a = 0;
				while(k < Lout[i].size() && a < Lin[reverse_permutation[Lout[i][j]]].size()){
					if(Lout[i][k] < Lin[reverse_permutation[Lout[i][j]]][a]){
						++a;
					}else if(Lout[i][k] > Lin[reverse_permutation[Lout[i][j]]][a]){
						++k;
					}else{
						label = 1;
						break;
					}
				}
				if (label == 0) {
					tmp_v.push_back(Lout[i][j]);
				}
			}
		}
		Lout[i].clear();
		Lout[i]=tmp_v;

		tmp_v.clear();
		if(Lin[i].size() != 0){
			for(unsigned j = 0; j < Lin[i].size(); j++) {
				unsigned label = 0;
				int k = j + 1;
				int a = 0;
				while(k < Lin[i].size() && a < Lout[reverse_permutation[Lin[i][j]]].size()){
					if(Lin[i][k] < Lout[reverse_permutation[Lin[i][j]]][a]){
						++a;
					}else if(Lin[i][k] > Lout[reverse_permutation[Lin[i][j]]][a]){
						++k;
					}else{
						label = 1;
						break;
					}
				}
				if (label == 0) {
					tmp_v.push_back(Lin[i][j]);
				}
			}
		}
		Lin[i].clear();
		Lin[i]=tmp_v;
	}
}

//构造TOL标签，即根据逆拓扑排序计算每个节点的L_out和根据拓扑排序计算每个节点的L_in标签
/*void construct_minV_BL(vector<vector<unsigned> > &Lout,
		vector<vector<unsigned> > &Lin,
		Graph *g,
		vector<unsigned> topo_order,
		vector<unsigned> reverse_topo_order,
		vector<unsigned> permutation,
		vector<vector<unsigned> > next,
		vector<vector<unsigned> > prior)
{
    Lout = vector<vector<unsigned> >(g->num_nodes());
    Lin = vector<vector<unsigned> >(g->num_nodes());

	vector<unsigned> reverse_permutation(g->num_nodes());
	for(unsigned i = 0; i < permutation.size(); ++i) {
		reverse_permutation[permutation[i]] = i;
	}

	//计算Lout
	for (unsigned i = 0; i < reverse_topo_order.size(); i++) {
		for(unsigned j = 0; j < next[reverse_topo_order[i]].size(); ++j){
			unsigned temp = next[reverse_topo_order[i]][j];
			for(unsigned k = 0; k < Lout[temp].size(); ++k){
				if(!count(Lout[reverse_topo_order[i]].begin(),Lout[reverse_topo_order[i]].end(),Lout[temp][k])
				   && Lout[temp][k] < permutation[reverse_topo_order[i]]){
                    Lout[reverse_topo_order[i]].push_back(Lout[temp][k]);
				}
			}
			if(permutation[temp] < permutation[reverse_topo_order[i]]){
				if(!count(Lout[reverse_topo_order[i]].begin(), Lout[reverse_topo_order[i]].end(),
						  permutation[temp])){
                    Lout[reverse_topo_order[i]].push_back(permutation[temp]);
				}
			}else if(!count(Lout[reverse_topo_order[i]].begin(), Lout[reverse_topo_order[i]].end(),
							permutation[reverse_topo_order[i]])){
                Lout[reverse_topo_order[i]].push_back(permutation[reverse_topo_order[i]]);
			}
		}
	}

	//计算Lin
	for (unsigned i = 0; i < topo_order.size(); i++) {
		for(unsigned j = 0; j < prior[topo_order[i]].size(); ++j){
			unsigned temp = prior[topo_order[i]][j];
			for(unsigned k = 0; k < Lin[temp].size(); ++k){
				if(!count(Lin[topo_order[i]].begin(),Lin[topo_order[i]].end(),Lin[temp][k])
				   && Lin[temp][k] < permutation[topo_order[i]]){
                    Lin[topo_order[i]].push_back(Lin[temp][k]);
				}
			}
			if(permutation[temp] < permutation[topo_order[i]]) {
				if(!count(Lin[topo_order[i]].begin(),Lin[topo_order[i]].end(),permutation[temp])){
                    Lin[topo_order[i]].push_back(permutation[temp]);
				}
			}else if(!count(Lin[topo_order[i]].begin(), Lin[topo_order[i]].end(),
							permutation[topo_order[i]])){
                Lin[topo_order[i]].push_back(permutation[topo_order[i]]);
			}
		}
	}

	//按降序删除错误节点
	for(unsigned i = 0; i < g->num_nodes(); ++i){
		sort(Lout[i].rbegin(), Lout[i].rend());
		sort(Lin[i].rbegin(), Lin[i].rend());
		vector<unsigned>::iterator it_Lout;
		unsigned index_Lout = 1;
		vector<unsigned>::iterator it_Lin;
		unsigned index_Lin = 1;
		for(it_Lout = Lout[i].begin(); it_Lout != Lout[i].end(); ++it_Lout){
			for(unsigned j = index_Lout; j < Lout[i].size(); ++j){
				unsigned temp = Lout[i][j];
				if(find(Lin[reverse_permutation[*it_Lout]].begin(),
						Lin[reverse_permutation[*it_Lout]].end(),temp) !=
						Lin[reverse_permutation[*it_Lout]].end()){
					it_Lout = Lout[i].erase(it_Lout);
					--it_Lout;
					--index_Lout;
					break;
				}
			}
			++index_Lout;
		}
		for(it_Lin = Lin[i].begin(); it_Lin != Lin[i].end(); ++it_Lin){
			for(unsigned j = index_Lin; j < Lin[i].size(); ++j){
				unsigned temp = Lin[i][j];
				if(find(Lout[reverse_permutation[*it_Lin]].begin(),
						Lout[reverse_permutation[*it_Lin]].end(),temp) !=
						Lout[reverse_permutation[*it_Lin]].end()){
					it_Lin = Lin[i].erase(it_Lin);
					--it_Lin;
					--index_Lin;
					break;
				}
			}
			++index_Lin;
		}
        sort(Lout[i].begin(), Lout[i].end());
        sort(Lin[i].begin(), Lin[i].end());
	}
}*/

//计算每个节点和所有节点的TOL标签总数，即Lout + Lin
void labelingnums(Graph *g, vector<vector<unsigned> > L_out_TOL, vector<vector<unsigned> > L_in_TOL,
		vector<unsigned> &onenode_labelingnums, unsigned &allnodes_labelingnums){
	allnodes_labelingnums = 0;
	for(unsigned i = 0; i < g->num_nodes(); ++i){
		unsigned count_onenode_labelingnums = 0;
		for(unsigned j = 0; j < L_out_TOL[i].size(); ++j){
			++count_onenode_labelingnums;
		}
		for(unsigned j = 0; j < L_in_TOL[i].size(); ++j){
			++count_onenode_labelingnums;
		}
		onenode_labelingnums.push_back(count_onenode_labelingnums);
		allnodes_labelingnums += count_onenode_labelingnums;
	}
}

//计算拓扑折叠度
void topo_fold_degree(unsigned num, unsigned N, unsigned &topoFoldDegree){
    if(N <= 2){
        topoFoldDegree = 1;
        return;
    }else if(num == ceil(N * 1.0 / 2 )){
        topoFoldDegree = 1;
    }else if(num < ceil(N * 1.0 / 2 )){
        topo_fold_degree(num, ceil(N * 1.0 / 2) - 1, topoFoldDegree);
        topoFoldDegree += 1;
    }else{
        topo_fold_degree(num -ceil(N * 1.0 / 2), N -ceil(N * 1.0 / 2), topoFoldDegree) ;
        topoFoldDegree += 1;
    }
}

//计算每个节点的最长路径长度
void maxPathLen(Graph *g,
		vector<unsigned> topo_level,
		vector<unsigned> reverse_topo_level,
		vector<unsigned> &max_pathLen)
{
	max_pathLen.clear();
    for(unsigned i = 0; i < g->num_nodes(); ++i){
        max_pathLen.push_back(topo_level[i] + reverse_topo_level[i] + 1);
    }
}

bool cmp(tuple<unsigned, unsigned, unsigned> num1, tuple<unsigned, unsigned, unsigned> num2){
	double a=(float)get<2>(num1);
	double b=(float)get<2>(num2);

	double x,y;
//	x=a/pow(2,get<1>(num1));
//	y=b/pow(2,get<1>(num2));

	x=a/get<1>(num1);
	y=b/get<1>(num2);
	if(x > y){
		return 1;
	}else if(x < y){
		return 0;
	}else{
		if(get<0>(num1) < get<0>(num2)){
			return 1;
		}else{
			return 0;
		}
	}
}


//自定义比较函数，先按照最长路径长度进行排序，如果最长路径长度相等，再按照拓扑折叠度大小对节点排序，
//如果拓扑折叠度也相等，则按照节点id大小排
/*bool cmp(tuple<unsigned, unsigned, unsigned> num1, tuple<unsigned, unsigned, unsigned> num2){
	if(get<2>(num1) > get<2>(num2)){
		return 1;
	}else if(get<2>(num1) < get<2>(num2)){
		return 0;
	}else{
		if(get<1>(num1) < get<1>(num2)){
			return 1;
		}else if(get<1>(num1) > get<1>(num2)){
			return 0;
		}else{
			if(get<0>(num1) < get<0>(num2)){
				return 1;
			}else{
				return 0;
			}
		}
	}
}*/

//order最终存的是：先按照最长路径长度进行排序，如果最长路径长度相等，再按照拓扑折叠度大小对节点排序，
//如果拓扑折叠度也相等，则按照节点id大小排，这样的节点序列
void nodesPermutationNum(Graph *g, vector<unsigned> topo_level, vector<unsigned> max_pathLen,
						 vector<tuple<unsigned, unsigned, unsigned> > &order){
	order.clear();
	unsigned position;
	unsigned topoFoldDegree;
	for(unsigned i = 0; i < g->num_nodes(); ++i){
		position = topo_level[i] + 1;
		topo_fold_degree(position, max_pathLen[i], topoFoldDegree);
		order.push_back(make_tuple(i, topoFoldDegree, max_pathLen[i]));
	}
	sort(order.begin(), order.end(), cmp);
	//测试输出排序后的节点
//	for (unsigned i = 0; i < order.size(); ++i) {
//		cout << get<0>(order[i]) << " ";
//	}
//	cout << endl;
}


//每次更新前delta个节点的邻接表和逆邻接表
void update_graph(unsigned delta,vector<vector<unsigned> > &next,
		vector<vector<unsigned> > &prior,
                  vector<tuple<unsigned, unsigned, unsigned> > order)
{
    vector<vector<unsigned> > temp_next(g->num_nodes());
    vector<vector<unsigned> > temp_prior(g->num_nodes());
    vector<unsigned> temp_order;

    for(unsigned i = 0; i < order.size(); ++i) {
        temp_order.push_back(get<0>(order[i]));
    }
    vector<unsigned> reverse_temp_order(g->num_nodes());
    for(unsigned i = 0; i < temp_order.size(); ++i) {
        reverse_temp_order[temp_order[i]] = i;
    }

    for(unsigned i = 0; i < next.size(); ++i)
    {
    	if(next[i].size()!=0&&reverse_temp_order[i]>=delta)
    	{
			for(unsigned j = 0; j < next[i].size(); ++j){
				if(reverse_temp_order[next[i][j]] >= delta){
					temp_next[i].push_back(next[i][j]);
				}
			}
    	}
    }

    next.clear();
    next=temp_next;

    for(unsigned i = 0; i < prior.size(); ++i)
    {
    	if(prior[i].size()!=0&&reverse_temp_order[i]>=delta)
    	{
			for(unsigned j = 0; j < prior[i].size(); ++j){
				if(reverse_temp_order[prior[i][j]] >= delta){
					temp_prior[i].push_back(prior[i][j]);
				}
			}
    	}
    }

    prior.clear();
    prior=temp_prior;
}

//先计算一次排序，从图中删除前delta个节点，再把剩余节点进行排序，然后再从图中删除前delta个节点，再把剩余节点进行排序;依次进行
void allNodesOrder(Graph *g,
		unsigned delta,
		vector<vector<unsigned> > next,
		vector<vector<unsigned> > prior,
		vector<tuple<unsigned, unsigned, unsigned> > order,
		vector<unsigned> &delta_nodesPermutation)
{
	for(unsigned j = 0; j < delta; ++j){
		delta_nodesPermutation.push_back(get<0>(order[j]));
	}
	for(unsigned i = 1; i < ceil(g->num_nodes() * 1.0 / delta); ++i)
	{
		update_graph(delta, next, prior, order);

		g->setNext(next);
		g->setPrior(prior);

		vector<pair<unsigned, unsigned> > node_array;
		vector<unsigned> topo_order;
		vector<unsigned> reverse_topo_order;

		for (unsigned j = 0; j < order.size() - delta; ++j) {
			node_array.push_back(make_pair(j, get<0>(order[j + delta])));
		}

	    vector<unsigned> reverse_temp_order(g->num_nodes());
	    for(unsigned j = 0; j < order.size() - delta; ++j)
	    {
	    	reverse_temp_order[get<0>(order[j + delta])]=j;
	    }

		order.clear();

		//计算从delta * i到order.size节点的拓扑排序
		vector<pair<unsigned, unsigned> > node_indegree;
		vector<unsigned> roots;
		for(unsigned j = 0; j < node_array.size(); ++j){
			node_indegree.push_back(make_pair(j, prior[node_array[j].second].size()));
		}
		stack<unsigned> s1;
		for (unsigned j = 0; j < node_array.size(); ++j) {
			if (node_indegree[j].second == 0) {
				roots.push_back(node_array[j].second);
				s1.push(node_array[j].second);
			}
		}
		g->setRoots(roots);
		while (!s1.empty()) {
			unsigned temp = s1.top();
			topo_order.push_back(temp);
			s1.pop();
			for (unsigned j = 0; j < next[temp].size(); j++) {
				unsigned temp2 = next[temp][j];
				unsigned index;
				index=reverse_temp_order[temp2];
				node_indegree[index].second--;
				if (node_indegree[index].second == 0) {
					s1.push(temp2);
				}
			}
		}

		//计算从delta * i到order.size节点的逆拓扑排序
		vector<pair<unsigned, unsigned> > node_degree;
		deque<unsigned> leaves;
		for(unsigned j = 0; j < node_array.size(); ++j) {
			node_degree.push_back(make_pair(j, next[node_array[j].second].size()));
		}
		stack<unsigned> s2;
		for (unsigned j = 0; j < node_array.size(); ++j) {
			if (node_degree[j].second == 0) {
				leaves.push_back(node_array[j].second);
				s2.push(node_array[j].second);
			}
		}
		g->setLeaves(leaves);
		while (!s2.empty()) {
			unsigned temp = s2.top();
			reverse_topo_order.push_back(temp);
			s2.pop();
			for (unsigned j = 0; j < prior[temp].size(); j++) {
				unsigned temp2 = prior[temp][j];
				unsigned index;
				index=reverse_temp_order[temp2];
				node_degree[index].second--;
				if (node_degree[index].second == 0) {
					s2.push(temp2);
				}
			}
		}

		//按照拓扑排序计算topo_level和按照逆拓扑排序计算reverse_topo_level
		vector<unsigned> topo_level(g->num_nodes());
		vector<unsigned> reverse_topo_level(g->num_nodes());
		for (unsigned j = 0; j < topo_order.size(); j++) {
			onenode_topolevel(g, topo_order[j], topo_level);
		}
		for (unsigned j = 0; j < reverse_topo_order.size(); j++) {
			onenode_reverse_topolevel(g, reverse_topo_order[j], reverse_topo_level);
		}

		//计算最长路径长度
		vector<unsigned> max_pathLen;
		for (unsigned j = 0; j < node_array.size(); ++j){
			max_pathLen.push_back(topo_level[node_array[j].second]+reverse_topo_level[node_array[j].second]+1);
		}
		unsigned position;
		unsigned topoFoldDegree;
		for(unsigned j = 0; j < node_array.size(); ++j){
			position = topo_level[node_array[j].second] + 1;
			topo_fold_degree(position, max_pathLen[j], topoFoldDegree);
			order.push_back(make_tuple(node_array[j].second, topoFoldDegree, max_pathLen[j]));
		}

		sort(order.begin(), order.end(), cmp);

		if(delta <= order.size()){
			for(unsigned i = 0; i < delta; ++i){
				delta_nodesPermutation.push_back(get<0>(order[i]));
			}
		}else{
			for(unsigned i = 0; i < order.size(); ++i){
				delta_nodesPermutation.push_back(get<0>(order[i]));
			}
		}
	}
}

void construct_minV_BL_k(vector<vector<unsigned> > &Lout_k,
                         vector<vector<unsigned> > &Lin_k,
                         unsigned k, Graph *g, vector<unsigned> topo_order,
                         vector<unsigned> reverse_topo_order,
                         vector<unsigned> permutation, vector<vector<unsigned> > next,
                         vector<vector<unsigned> > prior)
{
    Lout_k = vector<vector<unsigned> >(g->num_nodes());
    Lin_k = vector<vector<unsigned> >(g->num_nodes());

    construct_minV_BL(Lout_k, Lin_k, g,
                      topo_order, reverse_topo_order,
                      permutation, next, prior);

    //如果标签数大于k个，则将k个的标签数删掉
    for(unsigned i=0;i<g->num_nodes();i++)
    {
        if (Lout_k[i].size() > k) {
            Lout_k[i].erase(Lout_k[i].begin() + k,Lout_k[i].end());
        }
        if (Lin_k[i].size() > k) {
            Lin_k[i].erase(Lin_k[i].begin() + k,Lin_k[i].end());
        }
    }
}

bool is_lessthan_MinV_k(vector<unsigned> arr1, vector<unsigned> arr2){
    for(unsigned i = 0; i < min(arr1.size(),arr2.size()); ++i){
        if(arr1[i] < arr2[i]){
            return true;
        }
        if(arr1[i] > arr2[i]){
            return false;
        }
    }
    if(arr1.size()>arr2.size())
    {
        return false;
    }
    return true;
}

