/*
 * dbwr.c
 *
 *  Created on: 9 août 2016
 *      Author: Andy
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "../Headers/tableOperations.h"
#include "../Headers/LRUCache.h"



void scan()
{

	if(file->nbExtension == 0 || file->premier == NULL || file->dernier == NULL)
		return;


	FILE *fichier;
	fichier = fopen("data.dbf", "rb+");

	FNode *fn;
	fn = malloc(sizeof(FNode));

	extension *pE;
	pE = malloc(sizeof(extension));

	fn = file->dernier;

	int compteur = 0;

	while(fn != NULL)
	{
		//Si l'extension a été modifiée depuis le dernier scan
		if(fn->e.altered)
		{
			//On verrouille l'extension
			pthread_mutex_lock(&fn->lock);

			pE = &fn->e;

			pE->altered = 0;

			if(pE->id == -1)
			{
				//On cherche la longueur du fichier pour définir le numéro d'id de la nouvelle extension
				fseek(fichier, 0L, SEEK_END);
				unsigned long sz = ftell(fichier);
				pE->id = sz/sizeof(extension);
			}

			//On se déplace dans le fichier à l'endroit où doit être écrit l'extension
			fseek(fichier, pE->id*sizeof(extension), SEEK_SET);
			//Ecriture du bloc modifié dans le fichier
			fwrite(pE, sizeof(extension), 1, fichier);

			compteur++;

			//On libère le vérrou
			pthread_mutex_unlock(&fn->lock);
		}

		fn = fn->prev;
	} ;

	fseek(fichier, 0L, SEEK_END);
	unsigned long sz = ftell(fichier);
	file->nbExtInFichier = sz/sizeof(extension);

	//if(compteur>0)
	//	printf("compteur : %d\n", compteur);

	fclose(fichier);

	free(fn);

}


void startDbwr()
{
	while(1)
	{
		sleep(3);
		//printf("Ecriture\n");
		scan();
	}
}
