#include<iostream>
#include<algorithm>
#include<vector>
#include <sys/time.h>
#include<unordered_set>
#include<unordered_map>
#include <fstream>
#include <sstream>
#include<string>
#include "string.h"
#include <cstdlib>
#include <zstd.h>
#include "assert.h"
#include <memory>

using namespace std;

int num = 0;

struct JarData {
    bool def_lib_flag = false;
    ~JarData() {
        if (inner_matrix)
            delete inner_matrix;
        if (outer_matrix)
            delete outer_matrix;
    }

    int id = 0;
    int tpl_start_id = 0;
    int accessible_mtd_len = 0;
    vector <string> apis;
    unordered_map<string, int> def_map;
    unordered_map<string, int> lib_map;
    vector <vector<int>> *inner_matrix = nullptr;
    vector <vector<int>> *outer_matrix = nullptr;
};

struct QueryPair {
    string src;
    string target;
};

struct PathQueries {
    vector <string> dep_path;
    vector <QueryPair> queries;
    string target_jar;
};

struct Dataset {
    vector <PathQueries> paths;
};

string get_jar_path(const string &str) {
    size_t pos = str.rfind("@@");
    if (pos != std::string::npos) {
        return str.substr(pos + 2);
    }
    return "";
}

void extractQuery(const string &input, string &src, string &target) {
    size_t first_delim = input.find("@@");
    size_t second_delim = input.find("@@", first_delim + 1);
    src = string(input.begin(), input.begin() + first_delim);
    target = string(input.begin() + first_delim + 2, input.begin() + second_delim);
}

void read_queries(const string &path, Dataset &dataset, unordered_map <string, JarData> &jar_data_map) {
    for (int i = 1; i < 5; ++i) {
        auto pathQueries = make_shared<PathQueries>();
        string filename = path + "/graph_query" + to_string(i) + ".txt";
        ifstream file(filename.c_str(), std::ios::in);
        if (file.is_open()) {
            int n;
            file >> n;
            string line;
            getline(file, line);
            for (int j = 0; j < n; ++j) {
                getline(file, line);
                string jar_path = get_jar_path(line);
                pathQueries->dep_path.push_back(jar_path);
                JarData jar_data;
                jar_data_map[jar_path] = jar_data;
            }
            getline(file, line);
            pathQueries->target_jar = get_jar_path(line);
            JarData target_jar_data;
            jar_data_map[pathQueries->target_jar] = target_jar_data;
            for (int k = 0; k < 2500; ++k) {
                QueryPair queryPair;
                getline(file, line);
                extractQuery(line, queryPair.src, queryPair.target);
                pathQueries->queries.push_back(queryPair);
            }
            dataset.paths.push_back(*pathQueries);
        }
    }
}

void read_queries2(const string &path, Dataset &dataset, unordered_map <string, JarData> &jar_data_map) {
    auto pathQueries = make_shared<PathQueries>();
    string filename = path + "/test_graph.txt";
    ifstream file(filename.c_str(), std::ios::in);
    if (file.is_open()) {
        int n;
        file >> n;
        string line;
        getline(file, line);
        for (int j = 0; j < n; ++j) {
            getline(file, line);
            string jar_path = get_jar_path(line);
            pathQueries->dep_path.push_back(jar_path);
            JarData jar_data;
            jar_data_map[jar_path] = jar_data;
        }
        getline(file, line);
        pathQueries->target_jar = get_jar_path(line);
        JarData target_jar_data;
        jar_data_map[pathQueries->target_jar] = target_jar_data;
        for (int k = 0; k < 1; ++k) {
            QueryPair queryPair;
            getline(file, line);
            extractQuery(line, queryPair.src, queryPair.target);
            pathQueries->queries.push_back(queryPair);
        }
        dataset.paths.push_back(*pathQueries);
    }
}

void readFile(unordered_map<string, int> &map, string &path, vector <string> &apis, int &id) { //id是否可以
    ifstream file(path.c_str(), std::ios::in);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            map[line] = id;
            ++id;
            apis.push_back(string(line));
        }
        file.close();
    } else {
        // ?
    }
}

void readDefAndMap(const string &jar_path, JarData &jarData) {
    string not_public_file_path = jar_path + "/not_public_methods.txt";
    string public_file_path = jar_path + "/public_methods.txt";
    string tpl_file_path = jar_path + "/tpls_methods.txt";
    int id = 0;
    readFile(jarData.def_map, public_file_path, jarData.apis, id);
    jarData.accessible_mtd_len = id;
    readFile(jarData.def_map, not_public_file_path, jarData.apis, id);
    jarData.tpl_start_id = id;
    readFile(jarData.lib_map, tpl_file_path, jarData.apis, id);
}

vector <std::string> split(const std::string &s, char delimiter) {
    std::vector <std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void readMatrixData(string &path, vector <vector<int>> &matrix, int id) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << path << " Failed to open file." << std::endl;
        return;
    }
    file.seekg(0, std::ios::end);
    size_t compressed_size = file.tellg();
    file.seekg(0, std::ios::beg);

    // 读取压缩数据
    std::vector<char> compressed_data(compressed_size);
    file.read(compressed_data.data(), compressed_size);

    // 获取解压后的数据大小
    size_t decompressed_size = ZSTD_getFrameContentSize(compressed_data.data(), compressed_size);
    if (decompressed_size == ZSTD_CONTENTSIZE_ERROR) {
        std::cerr << "Failed to get decompressed size." << std::endl;
        exit(1);
    }

    // 分配解压缩后的内存
    std::vector<char> decompressed_data(decompressed_size);

    // 解压缩数据
    size_t actual_decompressed_size = ZSTD_decompress(decompressed_data.data(), decompressed_size,
                                                      compressed_data.data(), compressed_size);

    if (actual_decompressed_size != decompressed_size) {
        std::cerr << "Decompression failed." << std::endl;
        exit(1);
    }
    vector <string> lines = split(std::string(decompressed_data.data(), actual_decompressed_size), '\n');
    for (string &line: lines) {
        auto numvec = split(line, ',');
        int src_id = atoi(numvec[0].c_str());
        if (src_id >= matrix.size()) continue;
        for (int j = 1; j < numvec.size(); ++j) {
            int tar_id = atoi(numvec[j].c_str()) - id;
            if (tar_id >= matrix.size()) continue;
            if (tar_id >= 0 && tar_id < matrix[src_id].size()) {
                matrix[src_id][tar_id] = 1;
            }
        }
    }
    file.close();
}

void getJarData(const string &jar_path, JarData &jarData, bool is_outer) {
    if (!jarData.def_lib_flag) {
        jarData.def_lib_flag = true;
        readDefAndMap(jar_path, jarData);
    }
    if (is_outer && !jarData.outer_matrix) {
        string outer_matrixPath = jar_path + "/reachable_graph/outer_graph.zst";
        jarData.outer_matrix = new vector <vector<int>>(jarData.accessible_mtd_len,
                                                        vector<int>(jarData.lib_map.size(), 0));
        readMatrixData(outer_matrixPath, *jarData.outer_matrix, jarData.tpl_start_id);
    } else if (!is_outer && !jarData.inner_matrix) {
        string inner_matrixPath = jar_path + "/reachable_graph/inner_graph.zst";
        jarData.inner_matrix = new vector <vector<int>>(jarData.accessible_mtd_len,
                                                        vector<int>(jarData.def_map.size(), 0));
        readMatrixData(inner_matrixPath, *jarData.inner_matrix, 0);
    }
}


void
multiplyMatrices1Reduced(const vector<int> &A, const JarData &jarData, unordered_set <string> &outputApi) {
    size_t numRows = jarData.inner_matrix->size();
    for (size_t i = 0; i < jarData.def_map.size(); ++i) {
        for (int x: A) {
            if (x >= numRows) continue; // 避免越界

            const auto &row = (*jarData.inner_matrix)[x];
            if (i >= row.size()) continue; // 避免越界

            if (row[i] == 1) {
                outputApi.insert(jarData.apis[i]);
                break;
            }
        }
    }
}

void
multiplyMatrices2Reduced(const vector<int> &A, const JarData &jarData, unordered_set <string> &outputApi) {
    if (A.size() == 0) {
        return;
    }
    for (size_t i = 0; i < jarData.lib_map.size(); ++i) {
        for (int x: A) {
            if ((*jarData.outer_matrix)[x][i] == 1) {
                outputApi.insert(jarData.apis[i + jarData.tpl_start_id]);
                break;
            }
        }
    }
}

void getOutput(unordered_set <string> &input, JarData &jarData) {
    vector<int> entrance;
    vector <string> toRemove;
    for (const auto &api: input) {
        if (jarData.def_map.find(api) != jarData.def_map.end()) {
            entrance.push_back(jarData.def_map[api]);
            toRemove.push_back(api);
        }
    }

    for (const auto &api: toRemove) {
        input.erase(api);
    }
    multiplyMatrices2Reduced(entrance, jarData, input);
}

void getReachApis(unordered_set <string> &inputApis, const vector <string> &path,
                  unordered_map <string, JarData> &jar_data_map) {
    for (size_t i = 0; i < path.size(); ++i) {
        JarData &jar = jar_data_map[path[i]];
        getJarData(path.at(i), jar_data_map[path[i]], true);
        getOutput(inputApis, jar_data_map[path[i]]);
        if (inputApis.size() == 0) {
            break;
        }
    }
}

void query(const unordered_set <string> &input_apis, const unordered_set <string> &target_apis, JarData &jarData,
           unordered_set <string> &reachApis) {
    auto output_api = make_shared < unordered_set < string >> ();
    vector<int> entrance;
    int count = 0;
    for (const auto &api: input_apis) {
        if (jarData.def_map.count(api)) {
            if (jarData.def_map[api] >= jarData.accessible_mtd_len) {
                 continue;
            }
            entrance.push_back(jarData.def_map[api]);
            output_api->insert(api);
        }
    }
    multiplyMatrices1Reduced(entrance, jarData, *output_api);
    for (const auto &api: *output_api) {
        if (target_apis.count(api)) {
            reachApis.insert(api);
            count++;
        }
    }
}

double target_time[4] = {0, 0, 0, 0};
int flag = 0;

double executeQuery(Dataset &dataset, unordered_map <string, JarData> &jar_data_map, int &res) {
    // int reach_num = 0;
    timeval start_at, end_at;
    gettimeofday(&start_at, 0);
    unordered_set <string> reachApis;
    for (size_t i = 0; i < dataset.paths.size(); ++i) {
        const PathQueries &pathQueries = dataset.paths[i];
        auto input_apis = unordered_set<string>();
        auto target_apis = unordered_set<string>();
        for (const QueryPair &query: pathQueries.queries) {
            input_apis.insert(query.src);
            target_apis.insert(query.target);
        }
        getReachApis(input_apis, pathQueries.dep_path, jar_data_map);
        if (input_apis.size() == 0) {
            continue;
        } else {
            timeval start_at1, end_at1;
            const string &target_jar = pathQueries.target_jar;
            gettimeofday(&start_at1, 0);
            getJarData(target_jar, jar_data_map[target_jar], false);
            gettimeofday(&end_at1, 0);
            if (flag != 4) {
                target_time[i] =
                        (end_at1.tv_sec - start_at1.tv_sec) * 1000 + double(end_at1.tv_usec - start_at1.tv_usec) / 1000;
                ++flag;
            }
            query(input_apis, target_apis, jar_data_map[target_jar], reachApis);
        }
    }
    gettimeofday(&end_at, 0);
    cout << "reach_num:" << reachApis.size() << endl;
    res = reachApis.size();
    double time = (end_at.tv_sec - start_at.tv_sec) * 1000 + double(end_at.tv_usec - start_at.tv_usec) / 1000;
    return time;
}

unordered_map <string, JarData> jar_data_map;
Dataset dataset;

int main(int argc, char *argv[]) {
    std::ifstream input_file("/home/**/exp_result/query_project.txt");
    if (!input_file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return 1;
    }
    std::ofstream output_file("/home/**/exp_result/ReachCheck_result.txt");
    if (!output_file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return 1;
    }
    std::string line;
    while (std::getline(input_file, line)) {
        double time1;
        int res;
        flag = 0;
        vector <string> data = split(line, '@');
        string path = data[0];
        read_queries(path, dataset, jar_data_map);
        time1 = executeQuery(dataset, jar_data_map, res);
        for (int i = 0; i < 4; i++) {
            time1 += target_time[i];
        }
        dataset.paths.clear();
        jar_data_map.clear();
        cout << path << "@@" << time1 << endl;
        output_file << path << "@@" << time1 << "@@" << res << endl;
    }

    input_file.close();
    output_file.close();
}
