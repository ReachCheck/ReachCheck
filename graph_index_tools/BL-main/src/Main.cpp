/*
 * main.cpp
 *
 *  Created on: 2021年7月8日
 *      Author: vm
 */
#include "Basic.h"
#include "Graph.h"
#include "Index.h"
#include "BasicMethod.h"
#include "TopoLevel.h"
#include "genIndexMethods.h"
#include "queryMethods.h"
#include "transf.h"
#include "geneQuery.h"

void print_usage() {
	cout << "minBL Reachability Index" << endl;
	cout << "Usage: minBL -g <GRAPHFILE> -M <METHOD> -query <QUERYFILE> -index <INDEXFILE> [-k <SIZE>] [-d <DELTA>]" << endl;
	cout << "Description:\n"
         "-g <GRAPHFILE> is the name of the input graph.\n"
		 "-M <METHOD> is the method of constructing index or handling query or filtering or comparing.\n"
         "-query <QUERYFILE> contains a line of <src> <dest> <reachability> for each query.\n"
		 "-index <INDEXFILE> is the index of minBL.\n"
         "-k <SIZE> sets the value of parameter k, default value is 2.\n"
         "-d <DELTA> sets the value of parameter delta, default value is 2.\n"
	     "-tl 1 is used to compute topological level or handle query by topological level.\n"
	     "-rand 1 is used to compute the index of minBL or handle query by minBL index.\n"
	     "-randk 1 is used to compute the first k index of minBL or handle query by k-minBL index.\n"
	     "-randk 2 is used to compute the first k index of tfd-minBL.\n"
	     "-randk 3 is used to compute the first k index of ctfd-minBL.\n"
	     "-tfd 1 is used to compute the index of tfd-minBL or handle query by tfd-minBL index.\n"
	     "-tfdd 1 is used to compute the index of ctfd-minBL or handle query by ctfd-minBL index.\n" << endl;
}

vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

int main(int argc, char *argv[])
{
	const unsigned label_MinV_rand = 1;
  std::string exp_path = "/path/to/dataset/";
  std::ifstream input_file(exp_path+"query_project.txt");
  if (!input_file.is_open()) {
      std::cerr << "Failed to open file: " << exp_path+"query_project.txt"<< std::endl;
      return 1;
  }
  std::ofstream output_file(exp_path+"bl_result.txt");
  if (!output_file.is_open()) {
      std::cerr << "Failed to open file." <<exp_path+"bl_result.txt" <<std::endl;
      return 1;
  }
	std::string line;
	while (std::getline(input_file, line)) {
		int res = 0;
		double index_time = 0;
		double query_time = 0;

		unsigned label_topolevel=0;
		unsigned label_MinV_k=0;
		unsigned label_MinV_tfd=0;
		unsigned label_MinV_tfd_delta=0;
		unsigned label_ip = 0;
		unsigned k=2;
		unsigned delta=2;
		unsigned w=0;
		std::string M = "IQ";
		string h="";
		char * ipIndexFileName;
		char * f1=NULL;
		char * f2=NULL;
		char * f3=NULL;
		char * gene_fileName;
		vector<string> data = split(line,'@');
		string path = data[0];
		string graph_path = exp_path+"project/"+path+"/src_graph_dag.txt";
		string query_path = exp_path+"project/"+path+"/graph_query_dag.txt";
		string index_path = exp_path+"project/"+path+"/src_graph_dag.txt.index";
		char * input_graph_filename = (char*)graph_path.c_str();
		char * indexFileName = (char*)(index_path.c_str());
		char * queryFileName= (char*)query_path.c_str();
		std::cout<<path<<std::endl;
		for(auto& m : M){
			switch (m)
			{
				case 'I':
				{
					//1) load graph
					g = new Graph(input_graph_filename);
					//2) constructing index
					struct indexParaSettings p;
					p.delta=delta;
					p.k=k;
					p.g=g;
					p.input_graph_filename=input_graph_filename;
					p.label_MinV_k=label_MinV_k;
					p.label_MinV_rand=label_MinV_rand;
					p.label_MinV_tfd=label_MinV_tfd;
					p.label_MinV_tfd_delta=label_MinV_tfd_delta;
					p.label_topolevel=label_topolevel;
					index_time = genIndex(p);
				}break;
				case 'Q':
				{
					struct queryParaSettings p;
					p.indexFileName=indexFileName;
					p.k=k;
					p.label_MinV_k=label_MinV_k;
					p.label_MinV_rand=label_MinV_rand;
					p.label_MinV_tfd=label_MinV_tfd;
					p.label_MinV_tfd_delta=label_MinV_tfd_delta;
					p.label_topolevel=label_topolevel;
					p.queryFileName=queryFileName;
					p.input_graph_filename = input_graph_filename;
					// MinVLabel_RQ(p);
					MinVLabel_RQ_third(p,query_time,res);
				}break;
				case 'F':
				{
					struct queryParaSettings p;
					p.indexFileName=indexFileName;
					p.k=k;
					p.label_MinV_k=label_MinV_k;
					p.label_topolevel=label_topolevel;
					p.label_ip = label_ip;
					p.queryFileName=queryFileName;
					p.input_graph_filename = input_graph_filename;
					p.ipIndexFileName = ipIndexFileName;
					Filter_Test(p);
				}break;
				case 'C':
				{
					isEqual_queryResult(f1, f2);
				}break;
				case 'G':
				{
					string filename(gene_fileName);
					randQueryPair(input_graph_filename,1000000,filename);
					struct queryParaSettings p;
					p.indexFileName=indexFileName;
					p.k=k;
					p.label_MinV_k=label_MinV_k;
					p.label_MinV_rand=label_MinV_rand;
					p.label_MinV_tfd=label_MinV_tfd;
					p.label_MinV_tfd_delta=label_MinV_tfd_delta;
					p.label_topolevel=label_topolevel;
					p.queryFileName=gene_fileName;
					p.input_graph_filename = input_graph_filename;
					MinVLabel_makeR(p);
				}break;
				case 'N':{
					struct queryParaSettings p;
					p.indexFileName = indexFileName;
					p.k = k;
					p.label_MinV_k = label_MinV_k;
					p.label_MinV_rand = label_MinV_rand;
					p.label_MinV_tfd = label_MinV_tfd;
					p.label_MinV_tfd_delta = label_MinV_tfd_delta;
					p.label_topolevel = label_topolevel;
					p.input_graph_filename = input_graph_filename;
					p.queryFileName=gene_fileName;
					negativeQueryPair(p);
				}break;
				case 'P':{
					string fileName(gene_fileName);
					positiveQueryPair(g, 1000000, fileName);
				}
				case 'W':
				{
					string str(input_graph_filename);
					transfer(str,w);
				}break;
				case 'T':
				{
					compare_filter(f1, f2, f3);
				}break;
			}
		}
		output_file << path <<"@@"<<index_time<<"@@"<<query_time<<"@@"<<res<<endl;
		clear_index_data();
	}
	input_file.close();
	output_file.close();
	
}

