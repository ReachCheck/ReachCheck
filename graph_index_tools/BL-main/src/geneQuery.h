/*
 * geneQuery.h
 *
 *  Created by vm on Oct 23, 2021
 */

#ifndef GENEQUERY_H_
#define GENEQUERY_H_

#include "Basic.h"
#include "queryMethods.h"
#include "genIndexMethods.h"

void randQueryPair(char *gName,unsigned t,string filename);

void negativeQueryPair(struct queryParaSettings p);

vector<unsigned> get_BFSnext(Graph *g,unsigned root);

void positiveQueryPair(Graph *g, unsigned t, string fileName);

#endif /* GENEQUERY_H_ */
