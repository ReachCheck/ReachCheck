/*
 * genIndexMethods.cpp
 *
 *  Created by vm on Oct 14, 2021
 */

#include "genIndexMethods.h"

Graph *g = nullptr;
vector<vector<unsigned> > Lout_w,Lin_w,Lout_k_w,Lin_k_w,Lout_tfd_w,Lin_tfd_w,Lout_delta_w,Lin_delta_w;
vector<unsigned> topo_level_w,all_descendant_level_w,max_descendant_level_w,all_pre_level_w,max_pre_level_w;

vector<vector<unsigned> > Lout, Lin, Lout_k, Lin_k, Lout_tfd, Lin_tfd, Lout_delta, Lin_delta;
vector<unsigned> topo_level, all_descendant_level, max_descendant_level, all_pre_level, max_pre_level;
vector<vector<unsigned> > Lout_ip, Lin_ip;


void clear_index_data() {
	delete g;
	g = nullptr;

	Lout_w.clear();
	Lin_w.clear();
	Lout_k_w.clear();
	Lin_k_w.clear();
	Lout_tfd_w.clear();
	Lin_tfd_w.clear();
	Lout_delta_w.clear();
	Lin_delta_w.clear();
	topo_level_w.clear();
	all_descendant_level_w.clear();
	max_descendant_level_w.clear();
	all_pre_level_w.clear();
	max_pre_level_w.clear();
	Lout_w.clear();
	Lout_w.clear();
	Lout_w.clear();

	Lout.clear();
	Lin.clear();
	Lout_k.clear();
	Lin_k.clear();
	Lout_tfd.clear();
	Lin_tfd.clear();
	Lout_delta.clear();
	Lin_delta.clear();
	topo_level.clear();
	all_descendant_level.clear();
	max_descendant_level.clear();
	all_pre_level.clear();
	max_pre_level.clear();
	Lin_ip.clear();
	Lout_ip.clear();
}


//the number of nodes
unsigned get_numnodes(char* input_graph_filename){
    string line;
    unsigned n;
    ifstream stream(input_graph_filename);	//c_str() 以 char* 形式传回 string 内含字符串
    if (!stream.eof()) {    //eof（）返回true时是读到文件结束符0xFF，而文件结束符是最后一个字符的下一个字符。
        getline(stream, line);    //读取每行的数据
        istringstream iss(line, istringstream::in);    //用空格作为字符串分隔符，赋值n和m
        iss >> n;
    }
    stream.close();
    return n;
}

//处理文件打不开时的函数
void quit(){
    system("pause");
    exit(0);
}

//以delim分隔开s得到elems
void split(string s, char delim, vector<string> &elems){
    unsigned begin;
    unsigned end;
    begin = 0;
    end = s.find(delim);
    while(end < s.size()){
        elems.push_back(s.substr(begin, end-begin));
        begin = end + 1;
        end = s.find(delim, begin);
    }
    if(begin != s.size()){
        elems.push_back(s.substr(begin));
    }
}

double genIndex(struct indexParaSettings p)
{
	struct timeval tvs,tve;
	double span;

	string output_file_name;
	output_file_name=p.input_graph_filename;
	output_file_name=output_file_name+".index";

	unsigned label;

	vector<vector<unsigned> > next;
	next.clear();
	next.resize(g->num_nodes());
	vector<vector<unsigned> > prior;
	prior.clear();
	prior.resize(g->num_nodes());
	allnodes_next(g, next);
	allnodes_prior(g,prior);

	vector<unsigned> topo_order; //拓扑排序
	topological_sort(topo_order, p.g);
	vector<unsigned> reverse_topo_order; //逆拓扑排序
	reverse_topological_sort(reverse_topo_order, p.g);
	vector<unsigned> permutation; //排列
	compute_permutation(permutation, p.g);
	if(p.label_topolevel==1)
	{
		gettimeofday(&tvs,NULL);

//		unsigned max_level;
//		allnodes_topolevel(p.g, topo_order, topo_level_w, max_level);
		allnodes_topolevel(p.g, topo_order, topo_level_w);

        gettimeofday(&tve,NULL);
        span = (tve.tv_sec-tvs.tv_sec)*1000 + double(tve.tv_usec-tvs.tv_usec)/1000;

		label=5;
		ofstream os(output_file_name.c_str());
		os << '>' << label << endl;
		//write label_topolevel

		for(unsigned i = 0; i < p.g->num_nodes(); ++i)
		{
			os << topo_level_w[i] << " ";
		}
		os << endl;
		os.close();
	}

	if(p.label_MinV_rand==1)
	{
		gettimeofday(&tvs,NULL);
//	    construct_minV_BL(Lout_w, Lin_w, p.g, topo_order,
//	    		reverse_topo_order, permutation,p.g->get_next(),p.g->get_prior());
		construct_minV_BL(Lout_w, Lin_w, p.g, topo_order,
			    		reverse_topo_order, permutation,g->get_next(), g->get_prior());

        gettimeofday(&tve,NULL);
        span = (tve.tv_sec-tvs.tv_sec)*1000 + double(tve.tv_usec-tvs.tv_usec)/1000;

		label=1;
		ofstream os(output_file_name.c_str());
		os << '>' << label << endl;
		//write label_MinV_rand

		for(unsigned i = 0; i < p.g->num_nodes(); ++i)
		{
	        for(unsigned j = 0; j < Lout_w[i].size(); ++j){
	            os << Lout_w[i][j] << ' ';
	        }
	        os << endl;
	        for(unsigned j = 0; j < Lin_w[i].size(); ++j){
	            os << Lin_w[i][j] << ' ';
	        }
	        os << endl;
		}
		os.close();
	}
	if(p.label_MinV_k>=1)
	{
		gettimeofday(&tvs,NULL);

		if(p.label_MinV_k==1)
		{
//			construct_minV_BL_k(Lout_k_w, Lin_k_w, p.k, p.g, topo_order,
//		    		reverse_topo_order, permutation, p.g->get_next(), p.g->get_prior());
			construct_minV_BL_k(Lout_k_w, Lin_k_w, p.k, p.g, topo_order,
					    		reverse_topo_order, permutation,g->get_next(), g->get_prior());

            gettimeofday(&tve,NULL);
            span = (tve.tv_sec-tvs.tv_sec)*1000 + double(tve.tv_usec-tvs.tv_usec)/1000;
		}
		if(p.label_MinV_k==2)
		{
			vector<unsigned> reverse_topo_level;
			unsigned max_level;
			allnodes_topolevel(p.g, topo_order, topo_level_w);
			allnodes_reverse_topolevel(p.g, reverse_topo_order, reverse_topo_level);

			vector<unsigned> max_pathLen;
			maxPathLen(p.g, topo_level_w, reverse_topo_level, max_pathLen);
			vector<tuple<unsigned, unsigned, unsigned> > order;

			nodesPermutationNum(p.g, topo_level_w, max_pathLen, order);
			vector<unsigned> nodesPermutation;
			for (unsigned i = 0; i < order.size(); ++i) {
				nodesPermutation.push_back(get<0>(order[i]));
			}

//			construct_minV_BL(Lout_k_w, Lin_k_w, p.g, topo_order, reverse_topo_order,
//					nodesPermutation, p.g->get_next(), p.g->get_prior());
			construct_minV_BL(Lout_k_w, Lin_k_w, p.g, topo_order, reverse_topo_order,
								nodesPermutation, g->get_next(), g->get_prior());
		    for(unsigned i=0;i<g->num_nodes();i++)
		    {
				if (Lout_k_w[i].size() > p.k) {
					Lout_k_w[i].erase(Lout_k_w[i].begin() + p.k,Lout_k_w[i].end());
				}
				if (Lin_k_w[i].size() > p.k) {
					Lin_k_w[i].erase(Lin_k_w[i].begin() + p.k,Lin_k_w[i].end());
				}
		    }
            gettimeofday(&tve,NULL);
            span = (tve.tv_sec-tvs.tv_sec)*1000 + double(tve.tv_usec-tvs.tv_usec)/1000;
		}
		if(p.label_MinV_k==3)
		{
			vector<unsigned> reverse_topo_level;
			unsigned max_level;
			allnodes_topolevel(p.g, topo_order, topo_level_w);
			allnodes_reverse_topolevel(p.g, reverse_topo_order, reverse_topo_level);

			vector<unsigned> max_pathLen;
			maxPathLen(p.g, topo_level_w, reverse_topo_level, max_pathLen);
			vector<tuple<unsigned, unsigned, unsigned> > order;
			nodesPermutationNum(p.g, topo_level_w, max_pathLen, order);

			vector<unsigned> delta_nodesPermutation;
			allNodesOrder(p.g, p.delta, p.g->get_next(), p.g->get_prior(),
					order, delta_nodesPermutation);

			construct_minV_BL(Lout_k_w, Lin_k_w, g, topo_order, reverse_topo_order,
					delta_nodesPermutation, p.g->get_next(), p.g->get_prior());
			for(unsigned i=0;i<g->num_nodes();i++)
			{
				if (Lout_k_w[i].size() > p.k) {
					Lout_k_w[i].erase(Lout_k_w[i].begin() + p.k,Lout_k_w[i].end());
				}
				if (Lin_k_w[i].size() > p.k) {
					Lin_k_w[i].erase(Lin_k_w[i].begin() + p.k,Lin_k_w[i].end());
				}
			}
            gettimeofday(&tve,NULL);
            span = (tve.tv_sec-tvs.tv_sec)*1000 + double(tve.tv_usec-tvs.tv_usec)/1000;
		}

		label=2;
		ofstream os(output_file_name.c_str());
		os << '>' << label << endl;
		//write label_MinV_k

		for(unsigned i = 0; i < p.g->num_nodes(); ++i)
		{
	        for(unsigned j = 0; j < Lout_k_w[i].size(); ++j){
	            os << Lout_k_w[i][j] << ' ';
	        }
	        os << endl;
	        for(unsigned j = 0; j < Lin_k_w[i].size(); ++j){
	            os << Lin_k_w[i][j] << ' ';
	        }
	        os << endl;
		}
		os.close();
	}
	if(p.label_MinV_tfd==1)
	{
		gettimeofday(&tvs,NULL);

		vector<unsigned> reverse_topo_level;
		unsigned max_level;
		allnodes_topolevel(p.g, topo_order, topo_level_w);
		allnodes_reverse_topolevel(p.g, reverse_topo_order, reverse_topo_level);

		vector<unsigned> max_pathLen;
		maxPathLen(p.g, topo_level_w, reverse_topo_level, max_pathLen);
		vector<tuple<unsigned, unsigned, unsigned> > order;

		nodesPermutationNum(p.g, topo_level_w, max_pathLen, order);
		vector<unsigned> nodesPermutation;
		for (unsigned i = 0; i < order.size(); ++i) {
			nodesPermutation.push_back(get<0>(order[i]));
		}

//		construct_minV_BL(Lout_tfd_w, Lin_tfd_w, p.g, topo_order, reverse_topo_order,
//				nodesPermutation, p.g->get_next(), p.g->get_prior());
		construct_minV_BL(Lout_tfd_w, Lin_tfd_w, p.g, topo_order, reverse_topo_order,
						nodesPermutation, g->get_next(), g->get_prior());

        gettimeofday(&tve,NULL);
        span = (tve.tv_sec-tvs.tv_sec)*1000 + double(tve.tv_usec-tvs.tv_usec)/1000;

		label=3;
		ofstream os(output_file_name.c_str());
		os << '>' << label << endl;
		//write label_MinV_tfd

		for(unsigned i = 0; i < p.g->num_nodes(); ++i)
		{
			for(unsigned j = 0; j < Lout_tfd_w[i].size(); ++j){
				os << Lout_tfd_w[i][j] << ' ';
			}
			os << endl;
			for(unsigned j = 0; j < Lin_tfd_w[i].size(); ++j){
				os << Lin_tfd_w[i][j] << ' ';
			}
			os << endl;
		}
		os.close();
	}
	if(p.label_MinV_tfd_delta==1)
	{
		gettimeofday(&tvs,NULL);

		vector<unsigned> reverse_topo_level;
		unsigned max_level;
		allnodes_topolevel(p.g, topo_order, topo_level_w);
		allnodes_reverse_topolevel(p.g, reverse_topo_order, reverse_topo_level);

		vector<unsigned> max_pathLen;
		maxPathLen(p.g, topo_level_w, reverse_topo_level, max_pathLen);
		vector<tuple<unsigned, unsigned, unsigned> > order;
		nodesPermutationNum(p.g, topo_level_w, max_pathLen, order);

		vector<unsigned> delta_nodesPermutation;
//		allNodesOrder(p.g, p.delta, p.g->get_next(), p.g->get_prior(),
//				order, delta_nodesPermutation);
		allNodesOrder(p.g, p.delta, next, prior,
						order, delta_nodesPermutation);

//		construct_minV_BL(Lout_delta_w, Lin_delta_w, g, topo_order, reverse_topo_order,
//				delta_nodesPermutation, p.g->get_next(), p.g->get_prior());
		construct_minV_BL(Lout_delta_w, Lin_delta_w, g, topo_order, reverse_topo_order,
						delta_nodesPermutation, next, prior);
        gettimeofday(&tve,NULL);
        span = (tve.tv_sec-tvs.tv_sec)*1000 + double(tve.tv_usec-tvs.tv_usec)/1000;

		label=4;
		ofstream os(output_file_name.c_str());
		os << '>' << label << endl;
		//write label_MinV_tfd_delta

		for(unsigned i = 0; i < p.g->num_nodes(); ++i)
		{
			for(unsigned j = 0; j < Lout_delta_w[i].size(); ++j){
				os << Lout_delta_w[i][j] << ' ';
			}
			os << endl;
			for(unsigned j = 0; j < Lin_delta_w[i].size(); ++j){
				os << Lin_delta_w[i][j] << ' ';
			}
			os << endl;
		}
		os.close();
	}

	ofstream os(output_file_name.c_str());
	os << '<' << endl;
	os.close();
	return span;
}

void readIndex(struct readIndexParaSettings p){
	char* index_filename;
	index_filename=p.input_index_filename;

    ifstream in(index_filename);

    string buf;
    vector<string> tmp_Lout,tmp_Lin,tmp_Lout_k,tmp_Lin_k,tmp_Lout_tfd,tmp_Lin_tfd,tmp_Lout_delta,tmp_Lin_delta,tmp_topo_level;
    vector<unsigned> tmp2_Lout,tmp2_Lin,tmp2_Lout_k,tmp2_Lin_k,tmp2_Lout_tfd,tmp2_Lin_tfd,tmp2_Lout_delta,tmp2_Lin_delta;

    char* input_graph_filename;
    input_graph_filename = p.input_graph_filename;
    p.n = get_numnodes(input_graph_filename);

    if(in.is_open()==false){
        cout << "Cannot open " << index_filename << endl;
        quit();
    }
    while (!in.eof()){
        getline(in, buf);
        char t = buf.back();
        if(t == '<'){
            break;
        }
        if(t == '1'&&p.label_MinV_rand==1){
        	for(unsigned i = 0; i < p.n; ++i){
                getline(in, buf);
                split(buf, ' ', tmp_Lout);
                unsigned temp1;
                for(unsigned j = 0; j < tmp_Lout.size(); ++j){
                	temp1 = atoi(tmp_Lout[j].c_str());
                	tmp2_Lout.push_back(temp1);
                }
                Lout.push_back(tmp2_Lout);
                tmp_Lout.clear();
                tmp2_Lout.clear();

                getline(in, buf);
                split(buf, ' ', tmp_Lin);
                unsigned temp2;
				for(unsigned j = 0; j < tmp_Lin.size(); ++j){
					temp2 = atoi(tmp_Lin[j].c_str());
					tmp2_Lin.push_back(temp2);
				}
				Lin.push_back(tmp2_Lin);
                tmp_Lin.clear();
                tmp2_Lin.clear();
            }
        }
        if(t == '2'&&p.label_MinV_k==1){
            for(unsigned i = 0; i < p.n; ++i){
                getline(in, buf);
                split(buf, ' ', tmp_Lout_k);
                unsigned temp1;
				for(unsigned j = 0; j < tmp_Lout_k.size(); ++j){
					temp1 = atoi(tmp_Lout_k[j].c_str());
					tmp2_Lout_k.push_back(temp1);
				}
				Lout_k.push_back(tmp2_Lout_k);
                tmp_Lout_k.clear();
                tmp2_Lout_k.clear();

                getline(in, buf);
                split(buf, ' ', tmp_Lin_k);
                unsigned temp2;
				for(unsigned j = 0; j < tmp_Lin_k.size(); ++j){
					temp2 = atoi(tmp_Lin_k[j].c_str());
					tmp2_Lin_k.push_back(temp2);
				}
				Lin_k.push_back(tmp2_Lin_k);
                tmp_Lin_k.clear();
                tmp2_Lin_k.clear();
            }
        }
        if(t == '3'&&p.label_MinV_tfd==1){
            for(unsigned i = 0; i < p.n; ++i){
                getline(in, buf);
                split(buf, ' ', tmp_Lout_tfd);
                unsigned temp1;
				for(unsigned j = 0; j < tmp_Lout_tfd.size(); ++j){
					temp1 = atoi(tmp_Lout_tfd[j].c_str());
					tmp2_Lout_tfd.push_back(temp1);
				}
				Lout_tfd.push_back(tmp2_Lout_tfd);
                tmp_Lout_tfd.clear();
                tmp2_Lout_tfd.clear();

                getline(in, buf);
                split(buf, ' ', tmp_Lin_tfd);
                unsigned temp2;
				for(unsigned j = 0; j < tmp_Lin_tfd.size(); ++j){
					temp2 = atoi(tmp_Lin_tfd[j].c_str());
					tmp2_Lin_tfd.push_back(temp2);
				}
				Lin_tfd.push_back(tmp2_Lin_tfd);
                tmp_Lin_tfd.clear();
                tmp2_Lin_tfd.clear();
            }
        }
        if(t == '4'&&p.label_MinV_tfd_delta==1){
            for(unsigned i = 0; i < p.n; ++i){
                getline(in, buf);
                split(buf, ' ', tmp_Lout_delta);
                unsigned temp1;
				for(unsigned j = 0; j < tmp_Lout_delta.size(); ++j){
					temp1 = atoi(tmp_Lout_delta[j].c_str());
					tmp2_Lout_delta.push_back(temp1);
				}
				Lout_delta.push_back(tmp2_Lout_delta);
                tmp_Lout_delta.clear();
                tmp2_Lout_delta.clear();

                getline(in, buf);
                split(buf, ' ', tmp_Lin_delta);
                unsigned temp2;
				for(unsigned j = 0; j < tmp_Lin_delta.size(); ++j){
					temp2 = atoi(tmp_Lin_delta[j].c_str());
					tmp2_Lin_delta.push_back(temp2);
				}
				Lin_delta.push_back(tmp2_Lin_delta);
                tmp_Lin_delta.clear();
                tmp2_Lin_delta.clear();
            }
        }
        if(t == '5'&&p.label_topolevel==1){
        	getline(in, buf);
            split(buf, ' ', tmp_topo_level);
            unsigned temp1;
			for(unsigned j = 0; j < tmp_topo_level.size(); ++j){
				temp1 = atoi(tmp_topo_level[j].c_str());
				topo_level.push_back(temp1);
			}
			tmp_topo_level.clear();
        }
    }
    in.close();
}

//将IP标签图.index文件里的内容读出来，存到相应的vector里，以便进行可达性查询
void readIPIndex(string index_filename){
    ifstream in(index_filename.c_str());

    string buf, tmpbuf;
    unsigned begin, end;
    vector<string> tmp_Lout,tmp_Lin;
    vector<unsigned> tmp2_Lout,tmp2_Lin;

    if(in.is_open()==false){
        cout << "Cannot open " << index_filename << endl;
        quit();
    }

    while(getline(in, buf)){
        begin = buf.find(":");
        end = buf.find("#");
        tmpbuf = buf.substr(begin+2, end-(begin+2));
        split(tmpbuf, ' ', tmp_Lout);
        unsigned temp1;
        for(unsigned j = 0; j < tmp_Lout.size(); ++j){
            temp1 = atoi(tmp_Lout[j].c_str());
            tmp2_Lout.push_back(temp1);
        }
        Lout_ip.push_back(tmp2_Lout);
        tmp_Lout.clear();
        tmp2_Lout.clear();
        begin=end;
        end=buf.find('#', end+1);
        tmpbuf=buf.substr(begin+2, end-begin-2);
        split(tmpbuf, ' ', tmp_Lin);
        unsigned temp2;
        for(unsigned j = 0; j < tmp_Lin.size(); ++j){
            temp2 = atoi(tmp_Lin[j].c_str());
            tmp2_Lin.push_back(temp2);
        }
        Lin_ip.push_back(tmp2_Lin);
        tmp_Lin.clear();
        tmp2_Lin.clear();
    }
    in.close();
}

