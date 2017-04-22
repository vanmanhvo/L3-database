/*
 * dbOperations.h
 *
 *  Created on: 9 août 2016
 *      Author: Andy
 */

#include <pthread.h>

#ifndef HEADERS_DBOPERATIONS_H_
#define HEADERS_DBOPERATIONS_H_



typedef struct FNode
{
	struct FNode *prev;
	struct FNode *next;
	extension e;
	pthread_mutex_t lock;
} FNode;

typedef struct File
{
	int nbExtension;
	int nbTotalExtension;
	FNode *premier;
	FNode *dernier;
	int nbExtInFichier;
} File;

typedef struct HNode
{
	FNode *fn;
	struct HNode *next;
} HNode;

typedef struct Hash
{
	int capacite;
	HNode **array;
} Hash;


extern File *file;
extern Hash *hash;


FNode* newFNode(extension *e);
File* creerFile(int nbExtension);
Hash* creerHash(int capacite);
int isFilePleine();
int isFileVide();
void deFile();
void enFile(extension *e);
void deHash();
void enHash(FNode *fn);
HNode* ht_get(extension *e);
extension* getExtById(int id);
void referenceExtension(extension *e);
int chargerBlocs(char *nomTable);

#endif /* HEADERS_DBOPERATIONS_H_ */
