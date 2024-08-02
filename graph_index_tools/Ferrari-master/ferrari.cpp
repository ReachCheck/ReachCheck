//--------------------------------------------------------------------------------------------------
// Ferrari Reachability Index
// (c) 2012 Stephan Seufert. Web site: http://www.mpi-inf.mpg.de/~sseufert
//
// This work is licensed under the Creative Commons
// Attribution-Noncommercial-Share Alike 3.0 Unported License. To view a copy
// of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300,
// San Francisco, California, 94105, USA.
//--------------------------------------------------------------------------------------------------
#include "Index.h"
#include "Graph.h"
#include "IntervalList.h"
#include "Timer.h"
//--------------------------------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include <string>
using namespace std;
//--------------------------------------------------------------------------------------------------
void print_usage() {
  std::cout << "Ferrari Reachability Index" << std::endl;
  std::cout << "Usage: ferrari -g <GRAPHFILE> -q <QUERYFILE> -k <SIZE> [-s <SEEDS> ] [-L]" 
            << std::endl;
}
//--------------------------------------------------------------------------------------------------
void read_queries(const std::string& query_file,
    std::vector<std::pair<unsigned, unsigned> > *queries) {
  std::ifstream qf(query_file.c_str(), std::ios::in);
  if (qf.is_open()) {
    std::string line;
    std::set<unsigned> s_set, t_set;
    unsigned s, t;
    if (!qf.eof()) {
      while (qf.good()) {
        getline(qf, line);
        if (line.length()) {
          std::istringstream iss(line, std::istringstream::in);
          iss >> s;
          iss >> t;
          s_set.insert(s);
          t_set.insert(t);
          // queries->push_back(std::make_pair(s, t));
        }
      }
    }
    for (unsigned s : s_set) {
      for (unsigned t : t_set) {
          queries->push_back(std::make_pair(s, t));
      }
    }
  }
}

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

//--------------------------------------------------------------------------------------------------
// int main(int argc, char *argv[]) {
  // std::string graph_file = "", query_file = "";
  // unsigned seeds = 0, k = ~0u; 
  // bool global = true;
  // for (int i = 0; i < argc; ++i) {
  //   std::string arg = argv[i];
  //   if (arg == "-g") {         // graph file
  //     graph_file = argv[++i];
  //   } else if (arg == "-s") {  // number of seeds
  //     seeds = atoi(argv[++i]);
  //   } else if (arg == "-q") {  // query file
  //     query_file = argv[++i];
  //   } else if (arg == "-k") {  // index size constraint
  //     k = atoi(argv[++i]);
  //   } else if (arg == "-L") {  // local size buget
  //     global = false;
  //   } else if (arg == "-G") {  // global size budget
  //     global = true;
  //   }
  // }

  // if (graph_file == "" || query_file == "" || k==~0u) {
  //   print_usage();
  //   return 1;
  // }  

  // std::cout << "graph file: " << graph_file << std::endl;
  // std::cout << "query file: " << query_file << std::endl;
  // std::cout << "number of seeds: " << seeds << std::endl;
  // if (k < ~0u) {
  //   std::cout << "size constraint: " << k << std::endl;
  // } else {
  //   std::cout << "size constraint: none" << std::endl;
  // }

  // // parse graph
  // Graph *g = new Graph(graph_file);

  // // build index
  // Timer t1; t1.start();
  // Index bm(g, seeds, k, global);
  // bm.build();
  // double t_index = t1.stop();
  // std::cout << "assigned bitmaps (" << t_index << " ms)" << std::endl;

  // // assess index size
  // unsigned count = 0;
  // for (unsigned i = 0; i < g->num_nodes(); ++i) {
  //    if (bm.get_intervals(i)) {
  //      count += bm.get_intervals(i)->size();
  //    }
  // }
  // std::cout << "assigned " << count << " intervals" << std::endl;
  
  // // extract queries
  // std::vector<std::pair<unsigned, unsigned> > queries;
  // read_queries(query_file, &queries);
  // std::cout << "running queries" << std::endl;
  // unsigned reachable = 0;

  // // probe reachability index
  // t1.start();
  // reachable = 0;
  // for (std::vector<std::pair<unsigned, unsigned> >::const_iterator it =
  //     queries.begin(); it != queries.end(); ++it) {
  //   if (bm.reachable(it->first, it->second)) {  // reachability query
  //     ++reachable;
  //   }
  // }
  // double t_query = t1.stop();
  // std::cout << "query processing time (" << t_query << " ms)" << std::endl;
  // std::cout << bm.reset() << " expanded nodes" << std::endl;
  // std::cout << reachable << "/" << queries.size() << " reachable" << std::endl;

  // // calculate index size
  // unsigned interval_space = count * 4 * 2 + count;  // intervals + exactness flag
  // unsigned seed_space = (bm.used_seed_count() / 8) * 2 * g->num_nodes();
  // unsigned idspace = g->num_nodes() * 4;
  // unsigned filter_space = g->num_nodes() * 4 + g->num_nodes() * 4; // top order + top level
  // std::cout << "Index Size: " << interval_space + seed_space + idspace + filter_space
  //           << " bytes" << std::endl;
  // delete g;

int main() {
  unsigned seeds = 32, k = 5; 
  bool global = true;
  std::string exp_path = "/path/to/dataset/";
  std::ifstream input_file(exp_path+"query_project.txt");
  if (!input_file.is_open()) {
      std::cerr << "Failed to open file: " << exp_path+"query_project.txt"<< std::endl;
      return 1;
  }
  std::ofstream output_file(exp_path+"ferrari_result.txt");
  if (!output_file.is_open()) {
      std::cerr << "Failed to open file." <<exp_path+"ferrari_result.txt" <<std::endl;
      return 1;
  }
  std::string line;
  while (std::getline(input_file, line)) {
      std::set<int>results;
      std::vector<string> data = split(line,'@');
      string path = data[0];
      string graph_path = exp_path+"project/"+path+"/src_graph_dag.txt";
      string query_path = exp_path+"project/"+path+"/graph_query_dag.txt";
      cout<<path<<endl;
    // parse graph
      Graph *g = new Graph(graph_path);

      // build index
      Timer t1; 
      t1.start();
      Index bm(g, seeds, k, global);
      bm.build();
      double index_time = t1.stop();

      // extract queries
      std::vector<std::pair<unsigned, unsigned> > queries;
      read_queries(query_path , &queries);
      // unsigned reachable = 0;
      // probe reachability index
      t1.start();
      // reachable = 0;
      for (std::vector<std::pair<unsigned, unsigned> >::const_iterator it =
          queries.begin(); it != queries.end(); ++it) {
        if (bm.reachable(it->first, it->second)) {  // reachability query
          // ++reachable;
          results.insert(it->second);
        }
      }
      double query_time = t1.stop();
      output_file << path <<"@@"<<index_time<<"@@"<<query_time<<"@@"<<results.size()<<endl;
      delete g;
  }

  input_file.close();
  output_file.close();
}
//--------------------------------------------------------------------------------------------------
