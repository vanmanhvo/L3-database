/*
 * joinAlgorithms.h
 *
 *  Created on: 17 août 2016
 *      Author: Andy
 */

#ifndef HEADERS_JOINALGORITHMS_H_
#define HEADERS_JOINALGORITHMS_H_

typedef struct elemTri
{
	int extId;
	int blocId;
	int rowId;
	void *valeur;
} elemTri;

typedef struct elemT
{
	void *row;
	void *valeur;
} elemT;

typedef struct hElem
{
	elemT e;
	struct hElem *next;
} hElem;

typedef struct hashTable
{
	int capacite;
	hElem **array;
} hashTable;

typedef struct sInfos
{
	int numExt;
	int numBloc;
	int numElem;
	int rowId;
} sInfos;

void nestedLoop(clauseJoin *cJ, FNode *fn, FNode *fnJ, int nbExt, int nbExtJ, FNode *fnNew);
void mergeJoin(clauseJoin *cj, FNode *fn, FNode *fnJ, int nbExt, int nbExtJ, FNode *fnNew);
void hashJoin(clauseJoin *cj, FNode *fn, FNode *fnJ, int nbExt, int nbExtJ, FNode *fnNew);
elemT* tri(FNode *fn, char *nomCol, elemT *tabElem, int *nbElem, int nbExt, TYPE *t);
int comparerElements(TYPE t, size_t taille, extension *pE, extension *pE2, void *row1, void *row2, int decCol1, int decCol2);
void echanger(elemT *tableau, int a, int b);
void quickSort(elemT *tableau, int debut, int fin, colonne *c);
void newJoinTable(FNode *fn, extension *pE1, extension *pE2);
void insertIntoJoinTable(FNode *fn, extension *pE1, extension *pE2, void *row1, void *row2);
void joinTable(query *q, int tabNbExt[], char *tabNomTable[]);
void printJoinResult(FNode *fn, query *q);
hashTable* creerHashTable(int capacite);
void addToHashTable(hashTable *hT, extension *pE, void *row, int numCol, int decCol);
unsigned long hashString(char *str);
int hashValeur(void *valeur, TYPE t, size_t taille);

#endif /* HEADERS_JOINALGORITHMS_H_ */
