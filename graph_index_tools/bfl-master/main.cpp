#include <sys/time.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <utility>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <unistd.h>
#include "assert.h"

#ifndef K
#define K 5
#endif
#ifndef D
#define D (320 * K)
#endif

namespace bs {

using namespace std;

struct node {
  int N_O_SZ, N_I_SZ;
  int *N_O, *N_I;
  int vis;
  union {
    int L_in[K];
#if K > 8
    unsigned int h_in;
#else
    unsigned char h_in;
#endif
  };
  union {
    int L_out[K];
#if K > 8
    unsigned int h_out;
#else
    unsigned char h_out;
#endif
  };
  pair<int, int> L_interval;
};
vector<node> nodes;
int vis_cur, cur;

void read_graph(const char *filename) {
  timeval start_at, end_at;
  gettimeofday(&start_at, 0);
  // FILE *file = fopen(filename, "r");
  // // char header[] = "graph_for_greach";
  // // fscanf(file, "%s", header);
  // int n,m;
  // printf("%s\n",filename);
  // printf("%d???%d\n",&n,&m);
  // fscanf(file, "%d%d", &n,&m);
  // printf("%d!!!%d\n",n,m);
  // nodes.resize(n);
  // vector<vector<int>> N_O(n), N_I(n);
  // for (;;) {
  //   int u, v;
  //   if (feof(file) || fscanf(file, "%d", &u) != 1) {
  //     break;
  //   }
  //   printf("%d:",u);
  //   fgetc(file);
  //   while (!feof(file) && fscanf(file, "%d", &v) == 1) {
  //     printf("%d ",v);
  //     N_O[u].push_back(v);
  //     N_I[v].push_back(u);
  //   }
  //   printf("\n");
  //   fgetc(file);
  // }
  std::ifstream file(filename);
  if (!file.is_open()) {
      std::cerr << "cannot open: " << filename << '\n';
      exit(1);
  }
  int n,m;
  file>>n>>m;
  nodes.resize(n);
  vector<vector<int>> N_O(n), N_I(n);
  std::string line;
  while (std::getline(file, line)) {
    int u,v;
    istringstream is(line);
    is >> u;
    // std::cout << "u:" << u << endl;
    while(is>>v){
      // std::cout << "\t" << v;
      N_O[u].push_back(v);
      N_I[v].push_back(u);
    }
    // std::cout << "\n";
  }
  file.close();
  for (int u = 0; u < n; u++) {
    nodes[u].N_O_SZ = N_O[u].size();
    nodes[u].N_O = new int[N_O[u].size()];
    copy(N_O[u].begin(), N_O[u].end(), nodes[u].N_O);
    if(N_O[u].size() && nodes[u].N_O[0] > nodes.size()) 
      std::cout<<"";
    nodes[u].N_I_SZ = N_I[u].size();
    nodes[u].N_I = new int[N_I[u].size()];
    copy(N_I[u].begin(), N_I[u].end(), nodes[u].N_I);
  }
  gettimeofday(&end_at, 0);
  // printf("read time(graph): %.3fs\n",
  //        end_at.tv_sec - start_at.tv_sec +
  //            double(end_at.tv_usec - start_at.tv_usec) / 1000000);
}


static int c_in, r_in;
int h_in() {
  if (c_in >= (int)nodes.size() / D) {
    c_in = 0;
    r_in = rand();
  }
  c_in++;
  return r_in;
}

static int c_out, r_out;
int h_out() {
  if (c_out >= (int)nodes.size() / D) {
    c_out = 0;
    r_out = rand();
  }
  c_out++;
  return r_out;
}

void dfs_in(node &u) {
  u.vis = vis_cur;

  if (u.N_I_SZ == 0) {
    u.h_in = h_in() % (K * 32);
  } else {
    for (int i = 0; i < K; i++) {
      u.L_in[i] = 0;
    }

    for (int i = 0; i < u.N_I_SZ; i++) {
      node &v = nodes[u.N_I[i]];
      if (v.vis != vis_cur) {
        dfs_in(v);
      }
      if (v.N_I_SZ == 0) {
        int hu = v.h_in;
        u.L_in[(hu >> 5) % K] |= 1 << (hu & 31);
      } else {
        for (int j = 0; j < K; j++) {
          u.L_in[j] |= v.L_in[j];
        }
      }
    }

    int hu = h_in();
    u.L_in[(hu >> 5) % K] |= 1 << (hu & 31);
  }
}

void dfs_out(node &u) {
  u.vis = vis_cur;

  u.L_interval.first = cur++;

  if (u.N_O_SZ == 0) {
    u.h_out = h_out() % (K * 32);
  } else {
    for (int i = 0; i < K; i++) {
      u.L_out[i] = 0;
    }

    for (int i = 0; i < u.N_O_SZ; i++) {
      node &v = nodes[u.N_O[i]];
      if (v.vis != vis_cur) {
        dfs_out(v);
      }
      if (v.N_O_SZ == 0) {
        int hu = v.h_out;
        u.L_out[(hu >> 5) % K] |= 1 << (hu & 31);
      } else {
        for (int j = 0; j < K; j++) {
          u.L_out[j] |= v.L_out[j];
        }
      }
    }

    int hu = h_out();
    u.L_out[(hu >> 5) % K] |= 1 << (hu & 31);
  }

  u.L_interval.second = cur;
}

double index_construction() {
  timeval start_at, end_at;
  gettimeofday(&start_at, 0);

  vis_cur++;
  for (int u = 0; u < nodes.size(); u++) {
    if (nodes[u].N_O_SZ == 0) {
      dfs_in(nodes[u]);
    }
  }
  vis_cur++;
  cur = 0;
  for (int u = 0; u < nodes.size(); u++) {
    if (nodes[u].N_I_SZ == 0) {
      dfs_out(nodes[u]);
    }
  }
  gettimeofday(&end_at, 0);
  // printf("index time: %.3fs\n",
  //        end_at.tv_sec - start_at.tv_sec +
  //            double(end_at.tv_usec - start_at.tv_usec) / 1000000);
  double index_time = (end_at.tv_sec - start_at.tv_sec) * 1000 +double(end_at.tv_usec - start_at.tv_usec) / 1000;  
  return index_time;         
  // long long index_size = 0;
  // for (int u = 0; u < nodes.size(); u++) {
  //   index_size +=
  //       nodes[u].N_I_SZ == 0 ? sizeof(nodes[u].h_in) : sizeof(nodes[u].L_in);
  //   index_size +=
  //       nodes[u].N_O_SZ == 0 ? sizeof(nodes[u].h_out) : sizeof(nodes[u].L_out);
  //   index_size += sizeof(nodes[u].L_interval);
  // }
  // printf("index space: %.3fMB\n", double(index_size) / (1024 * 1024));
}

vector<pair<pair<int, int>, int>> queries;

void read_queries(const char *filename) {
  timeval start_at, end_at;
  gettimeofday(&start_at, 0);
  FILE *file = fopen(filename, "r");
  int u, v, r;
  set<int>u_values,v_values;
  while (fscanf(file, "%d%d", &u, &v) == 2) {
    u_values.insert(u);
    v_values.insert(v);
    queries.push_back(make_pair(make_pair(u, v), 0));
  }
  fclose(file);
  gettimeofday(&end_at, 0);
  // printf("read time(query): %.3fs\n",
  //        end_at.tv_sec - start_at.tv_sec +
  //            double(end_at.tv_usec - start_at.tv_usec) / 1000000);

  // for (int u : u_values) {
  //   for (int v : v_values) {
  //       queries.push_back(make_pair(make_pair(u, v), 0));
  //   }
  // }
}

bool reach(node &u, node &v) {
  if (u.L_interval.second < v.L_interval.second) {
    return false;
  } else if (u.L_interval.first <= v.L_interval.first) {
    return true;
  }

  if (v.N_I_SZ == 0) {
    return false;
  }
  if (u.N_O_SZ == 0) {
    return false;
  }
  if (v.N_O_SZ == 0) {
    if ((u.L_out[v.h_out >> 5] & (1 << (v.h_out & 31))) == 0) {
      return false;
    }
  } else {
    for (int i = 0; i < K; i++) {
      if ((u.L_out[i] & v.L_out[i]) != v.L_out[i]) {
        return false;
      }
    }
  }
  if (u.N_I_SZ == 0) {
    if ((v.L_in[u.h_in >> 5] & (1 << (u.h_in & 31))) == 0) {
      return false;
    }
  } else {
    for (int i = 0; i < K; i++) {
      if ((u.L_in[i] & v.L_in[i]) != u.L_in[i]) {
        return false;
      }
    }
  }

  for (int i = 0; i < u.N_O_SZ; i++) {
    if (nodes[u.N_O[i]].vis != vis_cur) {
      nodes[u.N_O[i]].vis = vis_cur;
      if (reach(nodes[u.N_O[i]], v)) {
        return true;
      }
    }
  }

  return false;
}

double run_queries() {
  timeval start_at, end_at;
  gettimeofday(&start_at, 0);
  int count = 0;
  for (vector<pair<pair<int, int>, int>>::iterator it = queries.begin();
       it != queries.end(); it++) {
    vis_cur++;
    int result = reach(nodes[it->first.first], nodes[it->first.second]);
    if(result == 1){
      it->second = 1;
      // printf("%d,%d\n",it->first.first, it->first.second);
        // printf("%d,",it->first.first);
    }
    // if (it->second == -1 || it->second == result) {
    //   it->second = result;
    // } else {
    //   count += 1;
    // }
  }
  gettimeofday(&end_at, 0);
  // printf("query time: %.3fms\n",
  //        (end_at.tv_sec - start_at.tv_sec) * 1000 +
  //            double(end_at.tv_usec - start_at.tv_usec) / 1000);
  double query_time = (end_at.tv_sec - start_at.tv_sec) * 1000 +double(end_at.tv_usec - start_at.tv_usec) / 1000; 
  return query_time;
}

int write_results() {
  int ncut = 0, pcut = 0, pos = 0;
  set<int>results;
  for (int i = 0; i < queries.size(); i++) {
    node &u = nodes[queries[i].first.first], &v = nodes[queries[i].first.second];
    bool pf = false, nf = false;

    if (u.L_interval.second < v.L_interval.second) {
      nf = true;
    } else if (u.L_interval.first <= v.L_interval.first) {
      pf = true;
    }

    if (v.N_I_SZ == 0) {
      nf = true;
    }
    if (u.N_O_SZ == 0) {
      nf = true;
    }
    if (v.N_O_SZ == 0) {
      if ((u.L_out[v.h_out >> 5] & (1 << (v.h_out & 31))) == 0) {
        nf = true;
      }
    } else {
      for (int i = 0; i < K; i++) {
        if ((u.L_out[i] & v.L_out[i]) != v.L_out[i]) {
          nf = true;
        }
      }
    }
    if (u.N_I_SZ == 0) {
      if ((v.L_in[u.h_in >> 5] & (1 << (u.h_in & 31))) == 0) {
        nf = true;
      }
    } else {
      for (int i = 0; i < K; i++) {
        if ((u.L_in[i] & v.L_in[i]) != u.L_in[i]) {
          nf = true;
        }
      }
    }

    if (queries[i].second && results.find(queries[i].first.second)==results.end()) {
    // if (queries[i].second) {
      results.insert(queries[i].first.second);
      // cout<<queries[i].first.first<<":"<<queries[i].first.second<<endl;
    }
    if (nf) {
      ncut++;
    }
    if (pf) {
      pcut++;
    }
  }
  // printf("reachable: %lu\n", results.size());
  return  results.size();
  // printf("answered only by label: %d + %d = %d\n", ncut, pcut, ncut + pcut);
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


int main(int argc, char *argv[]) {
  using namespace bs;

  std::string exp_path = "/path/to/dataset/";
  std::ifstream input_file(exp_path+"query_project.txt");
  if (!input_file.is_open()) {
      std::cerr << "Failed to open file: " << exp_path+"query_project.txt"<< std::endl;
      return 1;
  }
  std::ofstream output_file(exp_path+"bfl_result.txt");
  if (!output_file.is_open()) {
      std::cerr << "Failed to open file." <<exp_path+"bfl_result.txt" <<std::endl;
      return 1;
  }
  std::string line;
  while (std::getline(input_file, line)) {
      vector<string> data = split(line,'@');
      string path = data[0];
      string graph_path = exp_path+"project/"+path+"/src_graph_dag.txt";
      string query_path = exp_path+"project/"+path+"/graph_query_dag.txt";
      cout<<path<<endl;
      c_in = 0, r_in = rand(), c_out = 0, r_out = rand();
      read_graph(graph_path.c_str());
      double time2 = index_construction();
      read_queries(query_path.c_str());
      double time3 = run_queries();
      int reach_num = write_results();
      for (int u = 0; u < nodes.size(); ++u) {
        delete[] nodes[u].N_O;
        delete[] nodes[u].N_I;
      }
      nodes.clear();
      queries.clear();
      vis_cur = cur = 0;
      output_file <<path <<"@@"<<time2<<"@@"<<time3<<"@@"<<reach_num<<endl;
  }

  input_file.close();
  output_file.close();

  return 0;
}

