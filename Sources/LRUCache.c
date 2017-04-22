/*
 * dbOperations.c
 *
 *  Created on: 8 août 2016
 *      Author: Andy
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "../Headers/tableOperations.h"
#include "../Headers/LRUCache.h"
#include "../Headers/dbwr.h"

File *file;
Hash *hash;

FNode* newFNode(extension *e)
{
	FNode* temp = malloc(sizeof(FNode));
	temp->e = *e;

	temp->prev = temp->next = NULL;

	pthread_mutex_init(&temp->lock, NULL);

	return temp;
}

File* creerFile(int nbExtension)
{
	File* file = malloc(sizeof(File));

	file->nbExtension = 0;
	file->premier = file->dernier = NULL;

	file->nbExtInFichier = 0;

	file->nbTotalExtension = nbExtension;

	return file;
}


Hash* creerHash(int capacite)
{
	Hash* hash = malloc(sizeof(Hash));
	hash->capacite = capacite;

	hash->array = malloc(hash->capacite * sizeof(FNode));

	for(int i=0; i<hash->capacite; i++)
	{
		hash->array[i] = NULL;
	}

	return hash;
}

int isFilePleine()
{
	return file->nbExtension == file->nbTotalExtension;
}

int isFileVide()
{
	return file->dernier == NULL;
}

void deFile()
{
	if(isFileVide())
		return;

	if(file->premier == file->dernier)
		file->premier = NULL;

	FNode *temp = file->dernier;
	file->dernier = file->dernier->prev;

	if(file->dernier)
		file->dernier->next = NULL;

	free(temp);

	file->nbExtension--;
}

void enFile(extension *e)
{
	if(isFilePleine())
	{
		deHash();
		deFile();
	}

	FNode *temp = newFNode(e);

	temp->next = file->premier;

	if(isFileVide())
	{
		temp->next = NULL;
		file->dernier = file->premier = temp;
	}
	else
	{
		file->premier->prev = temp;
		file->premier = temp;
	}

	enHash(temp);

	file->nbExtension++;
}

void deHash()
{
	int h_id = file->dernier->e.id % hash->capacite;

	HNode *hn = hash->array[h_id];

	while(hn->next->fn->e.id != file->dernier->e.id && hn->next != NULL)
	{
		hn = hn->next;
	}

	if(hn->next->fn->e.id == file->dernier->e.id)
	{
		HNode *tmp = hn->next;

		if(hn->next->next != NULL)
			hn->next = hn->next->next;
		else
			hn->next = NULL;

		free(tmp);
	}
}

void enHash(FNode *fn)
{
	int h_id = 0;

	HNode *hn;
	hn = malloc(sizeof(HNode));

	hn->fn = fn;
	hn->next = NULL;

	h_id = fn->e.id % hash->capacite;

	if(hash->array[h_id] == NULL)
		hash->array[h_id] = hn;
	else
	{
		hn->next = hash->array[h_id];
		hash->array[h_id] = hn;
	}
}

HNode* ht_get(extension *e)
{
	if(e->id == -1)
		return NULL;

	int h_id = 0;

	HNode *hn;
	hn = malloc(sizeof(HNode));

	h_id = e->id % hash->capacite;


	hn = hash->array[h_id];

	if(hn == NULL)
	{
		return NULL;
	}

	while(hn->fn->e.id != e->id && hn != NULL && hn->next != NULL)
	{
		hn = hn->next;
	}


	if(hn->fn->e.id != e->id)
	{
		return NULL;
	}
	return hn;
}

extension* getExtById(int id)
{
	HNode *hn = malloc(sizeof(HNode));

	int h_id = id % hash->capacite;

	hn = hash->array[h_id];

	if(hn == NULL)
		return NULL;

	while(hn->fn->e.id != id && hn != NULL && hn->next != NULL)
	{
		hn = hn->next;
	}


	if(hn->fn->e.id != id)
		return NULL;

	return &hn->fn->e;
}

void referenceExtension(extension *e)
{
	HNode *hn = ht_get(e);
	FNode *reqExtension;

	if(hn != NULL && hn->fn != NULL)
		reqExtension = hn->fn;

	if(hn == NULL)
	{
		if(e->id == -1)
		{
			e->id = file->nbExtInFichier;
			file->nbExtInFichier++;
		}

		enFile(e);
	}

	else if(reqExtension != file->premier)
	{
		reqExtension->prev->next = reqExtension->next;
		if(reqExtension->next)
			reqExtension->next->prev = reqExtension->prev;

		if(reqExtension == file->dernier)
		{
			file->dernier = reqExtension->prev;
			file->dernier->next = NULL;
		}

		reqExtension->next = file->premier;
		reqExtension->prev = NULL;

		reqExtension->next->prev = reqExtension;

		file->premier = reqExtension;
	}
}


//Retourne le nombre de blocs chargés
int chargerBlocs(char *nomTable) {

	scan();

	//Ouverture du fichier
	FILE *fichier;
	fichier = fopen("data.dbf", "rb");



	extension *pE = malloc(sizeof(extension));
	int compteur = 0;

	//printf("Chargement des blocs dans le cache\n");

	//Vérification des noms des blocs et ajout dans le cache si ils correspondent au nom demandé
	while(fread(pE, sizeof(extension), 1, fichier) != (size_t)NULL)
	{
		if(pE != NULL && memcmp(pE->fp.h.t.nom, nomTable, strlen(nomTable)+1) == 0)
		{
			//Variable altered indicatrice de si l'extension a été modifiée fixée à 0
			pE->altered = 0;

			//Copie de l'extension dans le cache
			referenceExtension(pE);

			compteur++;
		}
	}

	fseek(fichier, 0L, SEEK_END);
	unsigned long sz = ftell(fichier);
	file->nbExtInFichier = sz/sizeof(extension);

	//Fermeture du fichier
	fclose(fichier);

	//Libération de la mémoire
	free(pE);

	//printf("%d blocs chargés en mémoire\n", compteur*8);

	return compteur;
}
