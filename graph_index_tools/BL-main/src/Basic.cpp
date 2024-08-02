/*
 * Basic.cpp
 *
 *  Created by vm on Oct 22, 2021
 */

#include "Basic.h"

void isEqual_queryResult(char * file1, char * file2){
    vector<unsigned> compare_file1;
    compare_file1.clear();
    vector<unsigned> compare_file2;
    compare_file2.clear();
    unsigned count_label0 = 0;
    unsigned count_label1 = 0;

    ifstream qf(file1, ios::in);
    if (qf.is_open()) {
        string line;
        unsigned label;
        if (!qf.eof()) {
            while (qf.good()) {
                getline(qf, line);
                if (line.length()) {
                    istringstream iss(line, istringstream::in);
                    iss >> label;
                    compare_file1.push_back(label);
                }
            }
        }
    }
    qf.close();

    ifstream rf(file2,ios::in);
    if(rf.is_open()){
        string line;
        unsigned label;
//        unsigned src,dst;
        if(!rf.eof()){
            while (rf.good()){
                getline(rf, line);
                if(line.length()){
                    istringstream iss(line, istringstream::in);
//                    iss >> src;
//                    iss >> dst;
                    iss >> label;
                    if(label == 0){
                        count_label0++;
                    }
                    if(label == 1){
                        count_label1++;
                    }
                    compare_file2.push_back(label);
                }
            }
        }
        cout << "count_label0 = " << count_label0 << endl;
        cout << "count_label1 = " << count_label1 << endl;
    }
    rf.close();

    unsigned count = 0;
    cout << compare_file1.size() << endl;
    cout << compare_file2.size() << endl;
    if(compare_file1.size() == compare_file2.size()){
        for(vector<unsigned>::const_iterator p = compare_file1.begin(),q = compare_file2.begin(); p!= compare_file1.end(); p++,q++)
        {
            if(*p != *q)
            {
            	cout << *p << "	" << *q << endl;
                count++;
            }
        }
    }
    cout << "count = " << count << endl;
}

void compare_filter(char * file1, char * file2, char * file3){
    vector<unsigned> compare_file1;
    compare_file1.clear();
    vector<unsigned> compare_file2;
    compare_file2.clear();
    vector<unsigned> compare_file3;
    compare_file3.clear();

    unsigned count_label000 = 0;
    unsigned count_label001 = 0;
    unsigned count_label010 = 0;
    unsigned count_label011 = 0;
    unsigned count_label100 = 0;
    unsigned count_label101 = 0;
    unsigned count_label110 = 0;
    unsigned count_label111 = 0;

    ifstream tlf(file1, ios::in);
    if (tlf.is_open()) {
        string line;
        unsigned label;
        if (!tlf.eof()) {
            while (tlf.good()) {
                getline(tlf, line);
                if (line.length()) {
                    istringstream iss(line, istringstream::in);
                    iss >> label;
                    compare_file1.push_back(label);
                }
            }
        }
    }
    tlf.close();

    ifstream minVf(file2,ios::in);
    if(minVf.is_open()){
        string line;
        unsigned label;
        if(!minVf.eof()){
            while (minVf.good()){
                getline(minVf, line);
                if(line.length()){
                    istringstream iss(line, istringstream::in);
                    iss >> label;
                    compare_file2.push_back(label);
                }
            }
        }
    }
    minVf.close();

    ifstream ipf(file3,ios::in);
    if(ipf.is_open()){
        string line;
        unsigned label;
        if(!ipf.eof()){
            while (ipf.good()){
                getline(ipf, line);
                if(line.length()){
                    istringstream iss(line, istringstream::in);
                    iss >> label;
                    compare_file3.push_back(label);
                }
            }
        }
    }
    ipf.close();

    cout << compare_file1.size() << endl;
    cout << compare_file2.size() << endl;
    cout << compare_file3.size() << endl;

    if(compare_file1.size() == compare_file2.size() && compare_file2.size() == compare_file3.size()){
        for(vector<unsigned>::const_iterator p = compare_file1.begin(),q = compare_file2.begin(),t=compare_file3.begin(); p!= compare_file1.end(); p++,q++,t++){
            if(*p == 0 && *q == 0 && *t == 0){
                count_label000++;
            }
            if(*p == 0 && *q == 0 && *t == 1){
                count_label001++;
            }
            if(*p == 0 && *q == 1 && *t == 0){
                count_label010++;
            }
            if(*p == 0 && *q == 1 && *t == 1){
                count_label011++;
            }
            if(*p == 1 && *q == 0 && *t == 0){
                count_label100++;
            }
            if(*p == 1 && *q == 0 && *t == 1){
                count_label101++;
            }
            if(*p == 1 && *q == 1 && *t == 0){
                count_label110++;
            }
            if(*p == 1 && *q == 1 && *t == 1){
                count_label111++;
            }
        }
    }
    cout << "count_label000 = " << count_label000 << endl;
    cout << "count_label001 = " << count_label001 << endl;
    cout << "count_label010 = " << count_label010 << endl;
    cout << "count_label011 = " << count_label011 << endl;
    cout << "count_label100 = " << count_label100 << endl;
    cout << "count_label101 = " << count_label101 << endl;
    cout << "count_label110 = " << count_label110 << endl;
    cout << "count_label111 = " << count_label111 << endl;
}
