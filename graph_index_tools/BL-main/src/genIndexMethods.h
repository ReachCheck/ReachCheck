/*
 * genIndexMethods.h
 *
 *  Created by vm on Oct 14, 2021
 */

#ifndef GENINDEXMETHODS_H_
#define GENINDEXMETHODS_H_
#include "Graph.h"
#include "BasicMethod.h"
#include "TopoLevel.h"
#include "Index.h"

//用extern修饰，整个项目都可以用下面定义的变量
extern Graph *g;
extern vector<vector<unsigned> > Lout_w,Lin_w,Lout_k_w,Lin_k_w,Lout_tfd_w,Lin_tfd_w,Lout_delta_w,Lin_delta_w;
extern vector<unsigned> topo_level_w, all_descendant_level_w, max_descendant_level_w;
extern vector<unsigned> all_pre_level_w, max_pre_level_w;

extern vector<vector<unsigned> > Lout, Lin, Lout_k, Lin_k, Lout_tfd, Lin_tfd, Lout_delta ,Lin_delta;
extern vector<unsigned> topo_level, all_descendant_level, max_descendant_level, all_pre_level, max_pre_level;
extern vector<vector<unsigned> > Lout_ip, Lin_ip;

void clear_index_data();

struct indexParaSettings
{
	unsigned label_topolevel;
	unsigned label_MinV_rand;
	unsigned label_MinV_k;
	unsigned label_MinV_tfd;
	unsigned label_MinV_tfd_delta;
	Graph *g;
	unsigned k,delta;
	string input_graph_filename;
};

struct readIndexParaSettings
{
    char* input_graph_filename;
    unsigned n;
	unsigned label_topolevel;
	unsigned label_MinV_rand;
	unsigned label_MinV_k;
	unsigned label_MinV_tfd;
	unsigned label_MinV_tfd_delta;
	char* input_index_filename;
};

unsigned get_numnodes(char* input_graph_filename);

void quit();

void split(string s, char delim, vector<string> &elems);

double genIndex(struct indexParaSettings p);

void readIndex(struct readIndexParaSettings p);

void readIPIndex(string index_filename);

#endif /* GENINDEXMETHODS_H_ */
