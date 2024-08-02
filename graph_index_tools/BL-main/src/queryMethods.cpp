/*
 * queryMethods.cpp
 *
 *  Created on: Oct 14, 2021
 *      Author: vm
 */
#include "queryMethods.h"

void read_queries_tuple(char * query_file,
		vector<tuple<unsigned, unsigned, unsigned> > &queries){
    ifstream qf(query_file,ios::in);
    if(qf.is_open()){
        string line;
        unsigned src, dest,label;
         set<unsigned> src_set, dest_set;
        if(!qf.eof()){
            while (qf.good()){
                getline(qf, line);
                if(line.length()){
                    istringstream iss(line, istringstream::in);
                    iss >> src;
                    iss >> dest;
                    iss >> label;
                    src_set.insert(src);
                    dest_set.insert(dest);
                    // queries.push_back(make_tuple(src, dest, label));
                }
            }
        }
        for (unsigned src : src_set) {
            for (unsigned dest : dest_set) {
                queries.push_back(make_tuple(src, dest, 0));
            }
        }
    }
}

//偏序关系，用于IP标签过滤
bool is_partial_order_relation(vector<unsigned> arr1, vector<unsigned> arr2){
    if(arr1 == arr2){
        return true;
    }
    vector<unsigned> temp;
    for(unsigned i = 0; i < arr2.size(); i++){
        unsigned j;
        for(j = 0; j < arr1.size(); j++){
            if(arr2[i] == arr1[j]){
                break;
            }
        }
        if(j == arr1.size()){
            temp.push_back(arr2[i]);
        }
    }
    if (!temp.empty()) {
        if (temp[0] > arr1[arr1.size() - 1]) {
            return true;
        }
    }else{
        return true;
    }
    return false;
}

void MinVLabel_RQ(struct queryParaSettings p)
{
    struct timeval tvs,tve;
    double span;

    //1)read index file
    struct readIndexParaSettings pp;
    pp.input_index_filename=p.indexFileName;
    pp.label_MinV_k=p.label_MinV_k;
    pp.label_MinV_rand=p.label_MinV_rand;
    pp.label_MinV_tfd=p.label_MinV_tfd;
    pp.label_topolevel=p.label_topolevel;
    pp.label_MinV_tfd_delta=p.label_MinV_tfd_delta;
    pp.input_graph_filename = p.input_graph_filename;
    readIndex(pp);
    //2)read query file
    vector < tuple<unsigned, unsigned, unsigned> > q;
    read_queries_tuple(p.queryFileName,q);

    gettimeofday(&tvs,NULL);
    cout <<"start..."<<endl;

	//3)processing queries
    for(unsigned i=0;i<q.size();i++)
    {
        unsigned u,v;
        u=get<0>(q[i]);
        v=get<1>(q[i]);
        //topo level filtering
        if(p.label_topolevel==1)
        {
            if(topo_level[u]>=topo_level[v])
            {
                continue;
            }
        }
        //MinV_k filtering
        if(p.label_MinV_k==1)
        {
            if((!is_lessthan_MinV_k(Lout_k[u], Lout_k[v])) ||
               (!is_lessthan_MinV_k(Lin_k[v], Lin_k[u])))
            {
                continue;
            }
        }
        //MinV iff for query
        if(p.label_MinV_rand==1)
        {
            if(u==v)
            {
                continue;
            }
            else
            {
                unsigned i_Lout = 0, j_Lin = 0;
                while(i_Lout < Lout[u].size() && j_Lin < Lin[v].size()){
                    if(Lout[u][i_Lout] < Lin[v][j_Lin])
                    {
                        ++i_Lout;
                    }
                    else if(Lin[v][j_Lin] < Lout[u][i_Lout])
                    {
                        ++j_Lin;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        if(p.label_MinV_tfd==1)
        {
            if(u==v)
            {
                continue;
            }
            else{
                unsigned i_Lout = 0, j_Lin = 0;
                while(i_Lout < Lout_tfd[u].size() && j_Lin < Lin_tfd[v].size()){
                    if(Lout_tfd[u][i_Lout] < Lin_tfd[v][j_Lin])
                    {
                        ++i_Lout;
                    }
                    else if(Lin_tfd[v][j_Lin] < Lout_tfd[u][i_Lout])
                    {
                        ++j_Lin;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        if(p.label_MinV_tfd_delta==1)
        {
            if(u==v)
            {
                continue;
            }
            else{
                unsigned i_Lout = 0, j_Lin = 0;
                while(i_Lout < Lout_delta[u].size() && j_Lin < Lin_delta[v].size()){
                    if(Lout_delta[u][i_Lout] < Lin_delta[v][j_Lin])
                    {
                        ++i_Lout;
                    }
                    else if(Lin_delta[v][j_Lin] < Lout_delta[u][i_Lout])
                    {
                        ++j_Lin;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
    cout << "end..."<< endl;
    gettimeofday(&tve,NULL);
    span = tve.tv_sec-tvs.tv_sec + (tve.tv_usec-tvs.tv_usec)/1000000.0;
    cout <<"time cost for processing queries: "<<span<<endl;
}

void Filter_Test(struct queryParaSettings p)
{
    struct timeval tvs,tve;
    double span;

	//1)read index file
	struct readIndexParaSettings pp;
	pp.input_index_filename=p.indexFileName;
	pp.label_MinV_k=p.label_MinV_k;
	pp.label_MinV_rand=p.label_MinV_rand;
	pp.label_MinV_tfd=p.label_MinV_tfd;
	pp.label_topolevel=p.label_topolevel;
	pp.label_MinV_tfd_delta=p.label_MinV_tfd_delta;
	pp.input_graph_filename = p.input_graph_filename;

	if(p.label_ip==1)
	{
		readIPIndex(p.ipIndexFileName);
	}
	else
	{
		readIndex(pp);
	}
	//2)read query file
	vector < tuple<unsigned, unsigned, unsigned> > q;
	read_queries_tuple(p.queryFileName,q);
	vector <unsigned> filter_level;
	vector <unsigned> filter_MinV_k;
	vector <unsigned> filter_ip;
	filter_level.clear();
	filter_MinV_k.clear();
    filter_ip.clear();

    gettimeofday(&tvs,NULL);
    cout <<"start..."<<endl;

	//3)processing queries
	for(unsigned i=0;i<q.size();i++)
	{
		unsigned u,v;
		u=get<0>(q[i]);
		v=get<1>(q[i]);
		//topo level filtering
		if(p.label_topolevel==1)
		{
			if(topo_level[u]>=topo_level[v])
			{
				filter_level.push_back(1);
			}
			else
			{
				filter_level.push_back(0);
			}
		}
		//MinV_k filtering
		if(p.label_MinV_k==1)
		{
			if((!is_lessthan_MinV_k(Lout_k[u], Lout_k[v])) || (!is_lessthan_MinV_k(Lin_k[v], Lin_k[u])))
			{
				filter_MinV_k.push_back(1);
			}
			else
			{
				filter_MinV_k.push_back(0);
			}
		}
        //IP filtering
        if(p.label_ip==1)
        {
            if((!is_partial_order_relation(Lout_ip[u], Lout_ip[v])) ||
               (!is_partial_order_relation(Lin_ip[v], Lin_ip[u])))
            {
                filter_ip.push_back(1);
            }
            else
            {
                filter_ip.push_back(0);
            }
        }
	}

    cout << "end..."<< endl;
    gettimeofday(&tve,NULL);
    span = tve.tv_sec-tvs.tv_sec + (tve.tv_usec-tvs.tv_usec)/1000000.0;
    cout <<"time cost for filtering: "<<span<<endl;

	if(p.label_topolevel==1)
	{
		string fileName;
		fileName=p.queryFileName;
		fileName=fileName+".fil_topolevel";
		ofstream os(fileName.c_str());

		for(unsigned i=0;i<filter_level.size();i++)
		{
			os << filter_level[i] << endl;
		}
		os.close();
	}
	if(p.label_MinV_k==1)
	{
		string fileName;
		fileName=p.queryFileName;
		fileName=fileName+".fil_MinV_k";
		ofstream os(fileName.c_str());

		for(unsigned i=0;i<filter_MinV_k.size();i++)
		{
			os << filter_MinV_k[i] << endl;
		}
		os.close();
	}
	if(p.label_ip==1)
	{
		string fileName;
		fileName=p.queryFileName;
		fileName=fileName+".fil_ip";
		ofstream os(fileName.c_str());

		for(unsigned i=0;i<filter_ip.size();i++)
		{
			os << filter_ip[i] << endl;
		}
		os.close();
	}
}

void MinVLabel_RQ_third(struct queryParaSettings p ,double& query_time,int& res)
{
	//1)read index file
    struct timeval tvs,tve;
	struct readIndexParaSettings pp;
	pp.input_index_filename=p.indexFileName;
	pp.label_MinV_k=p.label_MinV_k;
	pp.label_MinV_rand=p.label_MinV_rand;
	pp.label_MinV_tfd=p.label_MinV_tfd;
	pp.label_topolevel=p.label_topolevel;
	pp.label_MinV_tfd_delta=p.label_MinV_tfd_delta;
	pp.input_graph_filename = p.input_graph_filename;
	readIndex(pp);
	//2)read query file
	vector < tuple<unsigned, unsigned, unsigned> > q;
	read_queries_tuple(p.queryFileName,q);
	vector <unsigned> r;
	r.clear();
	r.resize(q.size());
	//3)processing queries
    gettimeofday(&tvs,NULL);
	for(unsigned i=0;i<q.size();i++)
	{
		unsigned u,v;
		u=get<0>(q[i]);
		v=get<1>(q[i]);
		//topo level filtering
		if(p.label_topolevel==1)
		{
			if(topo_level[u]>=topo_level[v])
			{
				r[i]=0;
				continue;
			}
		}
		//MinV_k filtering
		if(p.label_MinV_k==1)
		{
			if((!is_lessthan_MinV_k(Lout_k[u], Lout_k[v])) ||
						 (!is_lessthan_MinV_k(Lin_k[v], Lin_k[u])))
			{
				r[i]=0;
				continue;
			}
		}
		//MinV iff for query
		if(p.label_MinV_rand==1)
		{
			if(u==v)
			{
				r[i]=1;
			}
			else
			{
				unsigned i_Lout = 0, j_Lin = 0;
				while(i_Lout < Lout[u].size() && j_Lin < Lin[v].size()){
					if(Lout[u][i_Lout] < Lin[v][j_Lin])
					{
						++j_Lin;
					}
					else if(Lin[v][j_Lin] < Lout[u][i_Lout])
					{
						++i_Lout;
					}
					else
					{
						r[i]=1;
						break;
					}
				}
			}
//			r[i]=0;
		}
		if(p.label_MinV_tfd==1)
		{
            if(u==v)
            {
                r[i]=1;
            }
            else{
                unsigned i_Lout = 0, j_Lin = 0;
                while(i_Lout < Lout_tfd[u].size() && j_Lin < Lin_tfd[v].size()){
                    if(Lout_tfd[u][i_Lout] < Lin_tfd[v][j_Lin])
                    {
                        ++j_Lin;
                    }
                    else if(Lin_tfd[v][j_Lin] < Lout_tfd[u][i_Lout])
                    {
                        ++i_Lout;
                    }
                    else
                    {
                        r[i]=1;
                        break;
                    }
                }
            }
//			r[i]=0;
		}
		if(p.label_MinV_tfd_delta==1)
		{
            if(u==v)
            {
                r[i]=1;
            }
            else{
                unsigned i_Lout = 0, j_Lin = 0;
                while(i_Lout < Lout_delta[u].size() && j_Lin < Lin_delta[v].size()){
                    if(Lout_delta[u][i_Lout] < Lin_delta[v][j_Lin])
                    {
                        ++j_Lin;
                    }
                    else if(Lin_delta[v][j_Lin] < Lout_delta[u][i_Lout])
                    {
                        ++i_Lout;
                    }
                    else
                    {
                        r[i]=1;
                        break;
                    }
                }
            }
//			r[i]=0;
		}
	}
    gettimeofday(&tve,NULL);
	string fileName(p.queryFileName);
	// fileName=fileName+".r";
    // cout<<fileName<<endl;
	// ofstream os(fileName.c_str());
    unsigned u,v;
    set<unsigned> results;
	for(unsigned i=0;i<r.size();i++)
	{
        u=get<0>(q[i]);
		v=get<1>(q[i]);
        if(r[i] == 1){
            // cout<<u<<":"<<v<<endl;
            results.insert(v);
        }
	}
    // cout<<"可达数量:"<<results.size()<<endl;
	// os.close();
    query_time = (tve.tv_sec-tvs.tv_sec)*1000 + double(tve.tv_usec-tvs.tv_usec)/1000;
    res = results.size();
}

void MinVLabel_makeR(struct queryParaSettings p)
{
	//1)read index file
	struct readIndexParaSettings pp;
	pp.input_index_filename=p.indexFileName;
	pp.label_MinV_k=p.label_MinV_k;
	pp.label_MinV_rand=p.label_MinV_rand;
	pp.label_MinV_tfd=p.label_MinV_tfd;
	pp.label_topolevel=p.label_topolevel;
	pp.label_MinV_tfd_delta=p.label_MinV_tfd_delta;
	pp.input_graph_filename = p.input_graph_filename;
	readIndex(pp);
	//2)read query file
	vector < tuple<unsigned, unsigned, unsigned> > q;
	read_queries_tuple(p.queryFileName,q);
	vector <unsigned> r;
	r.clear();
	r.resize(q.size());

	//3)processing queries
	for(unsigned i=0;i<q.size();i++)
	{
		unsigned u,v;
		u=get<0>(q[i]);
		v=get<1>(q[i]);
		//topo level filtering
		if(p.label_topolevel==1)
		{
			if(topo_level[u]>=topo_level[v])
			{
				r[i]=0;
				continue;
			}
		}
		//MinV_k filtering
		if(p.label_MinV_k==1)
		{
			if((!is_lessthan_MinV_k(Lout_k[u], Lout_k[v])) ||
						 (!is_lessthan_MinV_k(Lin_k[v], Lin_k[u])))
			{
				r[i]=0;
				continue;
			}
		}
		//MinV iff for query
		if(p.label_MinV_rand==1)
		{
			if(u==v)
			{
				r[i]=1;
			}
			else
			{
				unsigned i_Lout = 0, j_Lin = 0;
				while(i_Lout < Lout[u].size() && j_Lin < Lin[v].size()){
					if(Lout[u][i_Lout] < Lin[v][j_Lin])
					{
						++i_Lout;
					}
					else if(Lin[v][j_Lin] < Lout[u][i_Lout])
					{
						++j_Lin;
					}
					else
					{
						r[i]=1;
						break;
					}
				}
			}
//			r[i]=0;
		}
		if(p.label_MinV_tfd==1)
		{
            if(u==v)
            {
                r[i]=1;
            }
            else{
                unsigned i_Lout = 0, j_Lin = 0;
                while(i_Lout < Lout_tfd[u].size() && j_Lin < Lin_tfd[v].size()){
                    if(Lout_tfd[u][i_Lout] < Lin_tfd[v][j_Lin])
                    {
                        ++i_Lout;
                    }
                    else if(Lin_tfd[v][j_Lin] < Lout_tfd[u][i_Lout])
                    {
                        ++j_Lin;
                    }
                    else
                    {
                        r[i]=1;
                        break;
                    }
                }
            }
//			r[i]=0;
		}
		if(p.label_MinV_tfd_delta==1)
		{
            if(u==v)
            {
                r[i]=1;
            }
            else{
                unsigned i_Lout = 0, j_Lin = 0;
                while(i_Lout < Lout_delta[u].size() && j_Lin < Lin_delta[v].size()){
                    if(Lout_delta[u][i_Lout] < Lin_delta[v][j_Lin])
                    {
                        ++i_Lout;
                    }
                    else if(Lin_delta[v][j_Lin] < Lout_delta[u][i_Lout])
                    {
                        ++j_Lin;
                    }
                    else
                    {
                        r[i]=1;
                        break;
                    }
                }
            }
//			r[i]=0;
		}
	}

	string fileName(p.queryFileName);
	fileName=fileName+".m";
	ofstream os(fileName.c_str());

	for(unsigned i=0;i<r.size();i++)
	{
		os << get<0>(q[i]) << " " << get<1>(q[i]) << " " <<r[i] << endl;
	}

	os.close();
}

bool makeNegativeQ(struct queryParaSettings p, unsigned u, unsigned v){
    //topo level filtering
    if(p.label_topolevel==1)
    {
        if(topo_level[u]>=topo_level[v])
        {
            return false;
        }
    }
    //MinV_k filtering
    if(p.label_MinV_k==1)
    {
        if((!is_lessthan_MinV_k(Lout_k[u], Lout_k[v])) ||
           (!is_lessthan_MinV_k(Lin_k[v], Lin_k[u])))
        {
            return false;
        }
    }
    //MinV iff for query
    if(p.label_MinV_rand==1)
    {
        unsigned i_Lout = 0, j_Lin = 0;
        while(i_Lout < Lout[u].size() && j_Lin < Lin[v].size()){
            if(Lout[u][i_Lout] < Lin[v][j_Lin])
            {
                ++i_Lout;
            }
            else if(Lin[v][j_Lin] < Lout[u][i_Lout])
            {
                ++j_Lin;
            }
            else
            {
                return true;
            }
        }
    }
    if(p.label_MinV_tfd==1)
    {
        unsigned i_Lout = 0, j_Lin = 0;
        while(i_Lout < Lout_tfd[u].size() && j_Lin < Lin_tfd[v].size()){
            if(Lout_tfd[u][i_Lout] < Lin_tfd[v][j_Lin])
            {
                ++i_Lout;
            }
            else if(Lin_tfd[v][j_Lin] < Lout_tfd[u][i_Lout])
            {
                ++j_Lin;
            }
            else
            {
                return true;
            }
        }
    }
    if(p.label_MinV_tfd_delta==1)
    {
        unsigned i_Lout = 0, j_Lin = 0;
        while(i_Lout < Lout_delta[u].size() && j_Lin < Lin_delta[v].size()){
            if(Lout_delta[u][i_Lout] < Lin_delta[v][j_Lin])
            {
                ++i_Lout;
            }
            else if(Lin_delta[v][j_Lin] < Lout_delta[u][i_Lout])
            {
                ++j_Lin;
            }
            else
            {
                return true;
            }
        }
    }
    return false;
}

