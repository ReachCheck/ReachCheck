/*
 * geneQuery.cpp
 *
 *  Created by vm on Oct 23, 2021
 */
#include "geneQuery.h"

void randQueryPair(char *gName,unsigned t,string filename)
{
	unsigned n=get_numnodes(gName);
	unsigned u,v;
	unsigned r=0;
	set<pair<unsigned,unsigned> >qset;
	time_t tm;
	srand((unsigned)time(&tm));
	ofstream output(filename.c_str());
	for(unsigned i=0; i<t; i++){
		u=(rand()+1)%n;
		v=(rand()+1)%n;
		if(qset.find(make_pair(u, v))==qset.end()){
            output<<u<<" "<<v<<" " << r <<"\n";
            qset.insert(make_pair(u, v));
		}
		else{
			i--;
			continue;
		}
	}
	output.close();
}

void negativeQueryPair(struct queryParaSettings p){
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

	unsigned n = get_numnodes(p.input_graph_filename);
	unsigned u,v;
	unsigned r = 0;
	unsigned count = 0;
	set<pair<unsigned,unsigned> > qset;

	time_t tm;
	srand((unsigned)time(&tm));

    string fileName(p.queryFileName);
    fileName=fileName+".n";
	ofstream output(fileName.c_str());

	while(count < 1000000){
        u = (rand() + 1) % n;
        v = (rand() + 1) %n ;
        if(qset.find(make_pair(u, v)) == qset.end()){
            //如果查询出来的结果是0，将此查询对添加到到输出文件
            if(!makeNegativeQ(p, u, v)){
                output<< u << " " << v << " " << r << "\n";
                qset.insert(make_pair(u, v));
                count++;
            }else{
                continue;
            }
        }
	}
	output.close();
}

vector<unsigned> get_BFSnext(Graph *g,unsigned root)
{
    queue<int> que;
    vector<bool> BFSflag(g->num_nodes(), false);
    vector<bool> QueFlag(g->num_nodes(), false);
    int now;

    //以该节点为根BFS
    BFSflag[root]=true;
    for(unsigned i=0; i<g->get_next()[root].size(); i++){
        if(BFSflag[g->get_next()[root][i]]==false){
            que.push(g->get_next()[root][i]);
            QueFlag[g->get_next()[root][i]]=true;
        }
    }
    //BFS
    while(que.empty()==false){
        now=que.front();
        que.pop();
        BFSflag[now]=true;
        for(unsigned i=0; i<g->get_next()[now].size(); i++){
            if((BFSflag[g->get_next()[now][i]]==false)&&(QueFlag[g->get_next()[now][i]]==false)){
                que.push(g->get_next()[now][i]);
                QueFlag[g->get_next()[now][i]]=true;
            }
        }
    }
    vector<unsigned>BFSnext;
    for(unsigned i=0;i<g->num_nodes();i++){
        if(BFSflag[i]==true)
            BFSnext.push_back(i);
    }
    return BFSnext;
}

void positiveQueryPair(Graph *g,unsigned t, string fileName)
{
    unsigned n = g->num_nodes();
    unsigned u, v, x;
    set<unsigned>uset;
    set<pair<unsigned,unsigned> >qset;
    vector<unsigned> BFSnext;
    time_t tm;
    srand((unsigned)time(&tm));

    fileName=fileName+".p";
    ofstream output(fileName.c_str());
    for(unsigned i=0; i<t; ){
        u=(rand()+1)%n;
        x=0;
        if(uset.find(u)==uset.end()){
            if((*(g->get_degrees()))[u]==0) {
                uset.insert(u);
                qset.insert(make_pair(u, u));
                output<<u<<" "<<u<<" "<<1<<"\n";
                i++;
                continue;
            }
            uset.insert(u);
            BFSnext.clear();
            BFSnext=get_BFSnext(g,u);
            x=BFSnext.size();
            if((x+i)>t)
                x = t - i;
            for(unsigned j=0;j<x; j++){
                v=BFSnext[j];
                if(qset.find(make_pair(u, v))==qset.end()){
                    output<<u<<" "<<v<<" "<<1<<"\n";
                    qset.insert(make_pair(u, v));
                }
            }
        }
        i=i+x;
    }
    output.close();
}
