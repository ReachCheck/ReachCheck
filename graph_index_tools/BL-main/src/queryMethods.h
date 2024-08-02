/*
 * queryMethods.h
 *
 *  Created on: Oct 14, 2021
 *      Author: vm
 */

#ifndef QUERYMETHODS_H_
#define QUERYMETHODS_H_

#include "Basic.h"
#include "genIndexMethods.h"
#include "Index.h"

struct queryParaSettings
{
    char * input_graph_filename;
	char * queryFileName;
	char * indexFileName;
    char * ipIndexFileName;
	unsigned k;

	unsigned label_topolevel;
	unsigned label_MinV_rand;
	unsigned label_MinV_k;
	unsigned label_MinV_tfd;
	unsigned label_MinV_tfd_delta;
	unsigned label_ip;
};

void read_queries_tuple(char* query_file, vector<tuple<unsigned, unsigned, unsigned> > &queries);

bool is_partial_order_relation(vector<unsigned> arr1, vector<unsigned> arr2);

void MinVLabel_RQ(struct queryParaSettings p);

void Filter_Test(struct queryParaSettings p);

void MinVLabel_RQ_third(struct queryParaSettings p,double& query_time,int& res);

void MinVLabel_makeR(struct queryParaSettings p);

bool makeNegativeQ(struct queryParaSettings p, unsigned u, unsigned v);


#endif /* QUERYMETHODS_H_ */
