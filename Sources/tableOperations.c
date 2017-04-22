/*
  * page.c
 *
 *  Created on: 10 juil. 2016
 *      Author: Andy
 *      Co-Author: Van
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>


#include "../Headers/tableOperations.h"
#include "../Headers/LRUCache.h"
#include "../Headers/dbwr.h"
#include "../Headers/parser.h"
#include "../Headers/joinAlgorithms.h"



/*
 * Fonction: creerExtension
 *
 * Créer une extension correspondant à une table passée en paramètre
 * Cette fonction est appelée lorsque tout les blocs d'une table sont pleins
 */
void creerExtension(extension *pE, char *nomTable) {

	FNode *fn;
	fn = malloc(sizeof(FNode));

	fn = file->premier;

	//On cherche le premier bloc possédant le même nom de table pour y récupérer les informations de la table
	/*
	while(memcmp(fn->e.fp.h.t.nom, nomTable, strlen(nomTable)+1) != 0) {
		fn = fn->next;
	}*/

	if(memcmp(fn->e.fp.h.t.nom, nomTable, strlen(nomTable)+1) != 0)
		return;

	//Création du 1er bloc de l'extension en ajoutant dans son header le nom de la table ainsi que ses informations
	memcpy(pE->fp.h.t.listeColonnes, fn->e.fp.h.t.listeColonnes, sizeof(fn->e.fp.h.t.listeColonnes));
	pE->fp.h.t.nbColonnes = fn->e.fp.h.t.nbColonnes;

	memcpy(pE->fp.h.t.nom, fn->e.fp.h.t.nom, sizeof(fn->e.fp.h.t.nom));

	pE->fp.h.t.tailleElem = fn->e.fp.h.t.tailleElem;

	//Initialisation des valeurs des autres Page (Bloc)
	for(int i=0;i<7;i++)
	{
		pE->p[i].data[0] = 0;
		pE->p[i].h.nbElements = 0;
	}

	pE->altered = 1;
	pE->id = file->nbExtInFichier;
	printf("id : %d\n", pE->id);
	file->nbExtInFichier++;

	referenceExtension(pE);

	//scan();
}


/*
 * Fonction: creerTable
 *
 * Créer une table ainsi qu'une extension correspondant au nom passé en paramètre
 */
void creerTable(query *q) {

	int nbExtensions = chargerBlocs(q->nomTable);

	if(nbExtensions != 0)
		return;



	//Allocation de la mémoire du pointeur sur l'extension
	extension *pE;
	pE = malloc(sizeof(extension));

	//Création du 1er bloc de l'extension en ajoutant dans son header le nom de la table ainsi que ses informations
	memcpy(pE->fp.h.t.nom, q->nomTable, sizeof(pE->fp.h.t.nom));

	//Nombre de colonne de la table
	pE->fp.h.t.nbColonnes = q->nbColonnes;

	pE->fp.h.t.tailleElem = 0;

	//Ecriture des colonnes dans le header
	for(int i=0;i<q->nbColonnes;i++)
	{
		//memcpy(pE->fp.h.t.listeColonnes[i].nom, c[i].nom, sizeof(c[i].nom));
		*(pE->fp.h.t.listeColonnes+i) = q->tabColonne[i];
		pE->fp.h.t.tailleElem += q->tabColonne[i].size;
	}


	//Initialisation des valeurs des autres Page (Bloc)
	for(int i=0;i<7;i++)
	{
		pE->p[i].data[0] = 0;
		pE->p[i].h.nbElements = 0;
	}

	pE->altered = 1;
	pE->id = file->nbExtInFichier;
	file->nbExtInFichier++;

	referenceExtension(pE);

	//scan();

	//Libération de la mémoire allouée
	//free(pE);

	printf("La table %s a été créée\n", q->nomTable);
}


void trouverPlace(char *nomTable, int *numBloc, int *numExt)
{

	//On charge les blocs en mémoire
	int nbExtension = chargerBlocs(nomTable);

	if(nbExtension == 0)
		return;

	extension *pE = malloc(sizeof(extension));
	FNode *fn = malloc(sizeof(FNode));


	int bloc;

	fn = file->premier;
	pE = &fn->e;

	while(memcmp(nomTable, pE->fp.h.t.nom, strlen(pE->fp.h.t.nom)+1) == 0)
	{
		//Vérification de la place disponible dans l'extension courante
		bloc = 0;

		while (bloc<7)
		{
			if(pE->p[bloc].h.nbElements * pE->fp.h.t.tailleElem + pE->fp.h.t.tailleElem <= (int)sizeof(pE->p[bloc].data))
			{
				*numBloc = bloc;
				break;
			}
			bloc++;
		}

		if(*numBloc != -1 || fn->next == NULL)
		{
			break;
		}

		fn = fn->next;
		pE = &fn->e;
	}

	//Si la table existe mais que tout les blocs sont pleins, création d'une nouvelle extension
	if(*numBloc == -1)
	{
		printf("extent\n");
		extension *pE2 = malloc(sizeof(extension));
		creerExtension(pE2, nomTable);
		pE = pE2;
		*numBloc = 0;
	}

	*numExt = pE->id;
}


/*
 * Fonction : insererValeurs
 *
 * Insere des valeurs données dans une table donnée
 */
void insererValeurs(query *q) {

	int h;
	extension *pE = malloc(sizeof(extension));

	int numBloc = -1;
	int numExt = 0;


	trouverPlace(q->nomTable, &numBloc, &numExt);
	pE = getExtById(numExt);


	//Verification que le nombre de valeurs passées correspond au nombre de colonnes de la table
	if(pE->fp.h.t.nbColonnes != q->nbValeurs)
		return;

	//Variable de décalage de chaque colonne
	int totalSize = 0;

	//Variable de décalage de chaque élément
	int decEcri;

	//printf("numbloc : %d\n", numBloc);



	h = 0;

	while(pE->p[numBloc].data[h * pE->fp.h.t.tailleElem] != (int)NULL) {
		h++;
	}

	decEcri = h*pE->fp.h.t.tailleElem;


	//Pour toutes les valeurs
	for(int i=0;i<q->nbValeurs;i++) {

		switch(pE->fp.h.t.listeColonnes[i].type) {
		case INT:
		{
			char *ptr;

			int x;
			x = strtol(q->tabValeurs[i], &ptr, 10);

			memcpy(pE->p[numBloc].data + decEcri + totalSize, &x, pE->fp.h.t.listeColonnes[i].size);

			totalSize += pE->fp.h.t.listeColonnes[i].size;

			break;
		}
		case CHAR:
		{
			memcpy(pE->p[numBloc].data + decEcri +totalSize, q->tabValeurs[i], strlen(q->tabValeurs[i])+1);

			totalSize += pE->fp.h.t.listeColonnes[i].size;


			break;
		}
		case FLOAT:
		{

			char *ptr;

			float x;
			x = strtof(q->tabValeurs[i], &ptr);

			memcpy(pE->p[numBloc].data + decEcri + totalSize, &x, pE->fp.h.t.listeColonnes[i].size);

			totalSize += pE->fp.h.t.listeColonnes[i].size;


			break;
		}

		case DOUBLE:
		{

			char *ptr;

			double x;
			x = strtod(q->tabValeurs[i], &ptr);

			memcpy(pE->p[numBloc].data + decEcri + totalSize, &x, pE->fp.h.t.listeColonnes[i].size);

			totalSize += pE->fp.h.t.listeColonnes[i].size;


			break;
		}

		case DATE:
		{
			struct tm *date;
			date = malloc(sizeof(struct tm));

			char *val;
			val = malloc(sizeof(q->tabValeurs[i]));

			char *ptr;


			//Jour
			val = strtok(q->tabValeurs[i], "/");
			if(val == NULL)
			{
				printf("Syntax error : invalid Date");
				return;
			}

			date->tm_mday = strtol(val, &ptr, 10);

			//Mois
			val = strtok(NULL, "/");
			if(val == NULL)
			{
				printf("Syntax error : invalid Date");
				return;
			}

			date->tm_mon = strtol(val, &ptr, 10);

			//Année
			val = strtok(NULL, "/");
			if(val == NULL)
			{
				printf("Syntax error : invalid Date");
				return;
			}

			date->tm_year = strtol(val, &ptr, 10);

			memcpy(pE->p[numBloc].data + decEcri + totalSize, date, pE->fp.h.t.listeColonnes[i].size);

			totalSize += pE->fp.h.t.listeColonnes[i].size;

			free(date);


			break;
		}
		/*case BIGINT:
		{

		}*/
		}

	}

	pE->p[numBloc].h.nbElements++;

	pE->altered = 1;

	free(q);
	//free(fn);

}






/*
 * Fonction: majTable
 *
 * Met à jour les éléments de la table
 */
void majTable(query *q)
{
	int nbExtensions;
	nbExtensions = chargerBlocs(q->nomTable);

	if(nbExtensions == 0)
		return;

	//Obligatoire sinon problème après l'appel de difftime
	char *test[q->where.nbConditions];
	for(int i=0;i<q->where.nbConditions;i++)
	{
		test[i] = malloc(strlen(q->where.tabNomColonne[i])+1);
		memcpy(test[i], q->where.tabNomColonne[i], strlen(q->where.tabNomColonne[i])+1);
	}



	//Pointeur sur extension courante
	extension *pE;
	pE = malloc(sizeof(extension));


	int totalSize = 0;

	int verif = 1;

	int x = 0;

	int h;

	FNode *fn;
	fn = malloc(sizeof(FNode));

	fn = file->premier;

	for(int exten=0; exten<nbExtensions; exten++)
	{

		pE = &fn->e;

		//Pour tout les blocs de l'extension
		for(int i=0;i<7;i++)
		{

			h = 0;


			//Pour tout les éléments du bloc
			for(int j=0;j<pE->p[i].h.nbElements;j++)
			{

				while(pE->p[i].data[h * pE->fp.h.t.tailleElem] == (int)NULL) {
					h++;
				}

				verif = 1;

				//S'il y a une clause where
				if(q->where.nbConditions != 0)
				{

					//Vérifications des conditions
					for(int k=0;k<q->where.nbConditions && verif == 1;k++)
					{

						totalSize = 0;

						x = 0;

						while(memcmp(pE->fp.h.t.listeColonnes[x].nom, q->where.tabNomColonne[k], strlen(q->where.tabNomColonne[k])+1) != 0 && x < pE->fp.h.t.nbColonnes)
						{
							totalSize += pE->fp.h.t.listeColonnes[x].size;
							x++;
						}

						if(memcmp(pE->fp.h.t.listeColonnes[x].nom, q->where.tabNomColonne[k], strlen(q->where.tabNomColonne[k])+1) != 0)
						{
							printf("Colonne inexistante\n");
							return;
						}



						switch(pE->fp.h.t.listeColonnes[x].type) {
						case INT:
						{
							char *ptr;

							int z;
							z = strtol(q->where.tabValeur[k], &ptr, 10);

							int *y;
							y = malloc(sizeof(int));
							memcpy(y, pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, sizeof(int));


							//Conditions
							if (memcmp(q->where.tabOperateur[k], "=", strlen("=")+1) == 0)
							{
								if(z != *y)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], "!=", strlen("!=")+1) == 0)
							{

								if(z == *y)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], ">", strlen(">")+1) == 0)
							{
								if(*y <= z)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], "<", strlen("<")+1) == 0)
							{
								if(*y >= z)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], ">=", strlen(">=")+1) == 0)
							{
								if(*y < z)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], "<=", strlen("<=")+1) == 0)
							{
								if(*y > z)
								{
									verif = 0;
								}
							}

							break;
						}
						case CHAR:
							//Conditions
							if (memcmp(q->where.tabOperateur[k], "=", strlen("=")+1) == 0)
							{

								if(memcmp(pE->p[i].data + h*pE->fp.h.t.tailleElem + totalSize, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1) != 0)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], "!=", strlen("!=")+1) == 0)
							{

								if(memcmp(pE->p[i].data + h*pE->fp.h.t.tailleElem + totalSize, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1) == 0)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], ">", strlen(">")+1) == 0)
							{
								if(memcmp(pE->p[i].data + h*pE->fp.h.t.tailleElem + totalSize, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1) <= 0)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], "<", strlen("<")+1) == 0)
							{
								if(memcmp(pE->p[i].data + h*pE->fp.h.t.tailleElem + totalSize, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1) >= 0)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], ">=", strlen(">=")+1) == 0)
							{
								if(memcmp(pE->p[i].data + h*pE->fp.h.t.tailleElem + totalSize, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1) < 0)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], "<=", strlen("<=")+1) == 0)
							{
								if(memcmp(pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, q->where.tabValeur[k], strlen(q->where.tabValeur[k]+1)) > 0)
								{
									verif = 0;
								}
							}

							break;
						case FLOAT:
						{
							char *ptr;

							float z;
							z = strtof(q->where.tabValeur[k], &ptr);

							float *y;
							y = malloc(sizeof(float));
							memcpy(y, pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, sizeof(float));


							//Conditions
							if (memcmp(q->where.tabOperateur[k], "=", strlen("=")+1) == 0)
							{
								if(z != *y)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], "!=", strlen("!=")+1) == 0)
							{

								if(z == *y)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], ">", strlen(">")+1) == 0)
							{
								if(*y <= z)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], "<", strlen("<")+1) == 0)
							{
								if(*y >= z)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], ">=", strlen(">=")+1) == 0)
							{
								if(*y < z)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], "<=", strlen("<=")+1) == 0)
							{
								if(*y > z)
								{
									verif = 0;
								}
							}

							break;
						}
						case DOUBLE:

						{
							char *ptr;

							double z;
							z = strtod(q->where.tabValeur[k], &ptr);

							double *y;
							y = malloc(sizeof(double));
							memcpy(y, pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, sizeof(double));


							//Conditions
							if (memcmp(q->where.tabOperateur[k], "=", strlen("=")+1) == 0)
							{
								if(z != *y)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], "!=", strlen("!=")+1) == 0)
							{

								if(z == *y)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], ">", strlen(">")+1) == 0)
							{
								if(*y <= z)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], "<", strlen("<")+1) == 0)
							{
								if(*y >= z)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], ">=", strlen(">=")+1) == 0)
							{
								if(*y < z)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], "<=", strlen("<=")+1) == 0)
							{
								if(*y > z)
								{
									verif = 0;
								}
							}

							break;
						}
						case DATE:
						{
							struct tm *newDate;
							newDate = malloc(sizeof(struct tm));

							char *val;
							val = malloc(sizeof(q->where.tabValeur[k]));

							char *ptr;

							char *copy;
							copy = malloc(strlen(q->where.tabValeur[k])+1);
							memcpy(copy, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1);

							//Jour
							val = strtok(copy, "/");
							if(val == NULL)
							{
								printf("Syntax error : invalid Date");
								return;
							}

							newDate->tm_mday = strtol(val, &ptr, 10);

							//Mois
							val = strtok(NULL, "/");
							if(val == NULL)
							{
								printf("Syntax error : invalid Date");
								return;
							}

							newDate->tm_mon = strtol(val, &ptr, 10);

							//Année
							val = strtok(NULL, "/");
							if(val == NULL)
							{
								printf("Syntax error : invalid Date");
								return;
							}

							newDate->tm_year = strtol(val, &ptr, 10);


							struct tm *date;
							date = malloc(sizeof(struct tm));
							memcpy(date, pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, sizeof(struct tm));

							newDate->tm_hour = newDate->tm_min = newDate->tm_sec = 0;
							date->tm_hour = date->tm_min = date->tm_sec = 0;

							double dif = difftime(mktime(date), mktime(newDate));


							//Conditions
							if (memcmp(q->where.tabOperateur[k], "=", strlen("=")+1) == 0)
							{

								if(dif != 0)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], "!=", strlen("!=")+1) == 0)
							{

								if(dif == 0)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], ">", strlen(">")+1) == 0)
							{
								if(dif <= 0)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], "<", strlen("<")+1) == 0)
							{
								if(dif >= 0)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], ">=", strlen(">=")+1) == 0)
							{
								if(dif < 0)
								{
									verif = 0;
								}
							}
							else if (memcmp(q->where.tabOperateur[k], "<=", strlen("<=")+1) == 0)
							{
								if(dif > 0)
								{
									verif = 0;
								}
							}


							free(newDate);
							free(date);

							free(copy);

							break;
						}
						}

						//Cas où OR est présent
						if(verif == 1 && k == q->where.nbConditions-2)
						{
							if(memcmp(q->where.tabDelim[k], "OR", strlen("OR")) == 0)
							{
								break;
							}
						}

						if(verif == 0 && k<q->where.nbConditions-1)
						{
							if(memcmp(q->where.tabDelim[k], "OR", strlen("OR")) == 0)
							{
								verif = 1;
							}
						}

					}

				}


				//Si l'élément correspond aux conditions de la clause WHERE
				if(verif == 1)
				{

					//Pour toutes les valeurs à mettre à jour
					for(int k=0;k<q->nbValeurs;k++)
					{

						totalSize = 0;

						int x = 0;

						while(memcmp(pE->fp.h.t.listeColonnes[x].nom, q->tabColonne[k].nom, strlen(q->tabColonne[k].nom)+1) != 0 && x < pE->fp.h.t.nbColonnes)
						{
							totalSize += pE->fp.h.t.listeColonnes[x].size;
							x++;
						}

						if(memcmp(pE->fp.h.t.listeColonnes[x].nom, q->tabColonne[k].nom, strlen(q->tabColonne[k].nom)+1) != 0)
						{
							printf("Colonne inexistante\n");
							return;
						}


						switch(pE->fp.h.t.listeColonnes[x].type) {
						case INT:
						{
							char *ptr;

							int a;
							a = strtol(q->tabValeurs[k], &ptr, 10);

							memcpy(pE->p[i].data + h*pE->fp.h.t.tailleElem + totalSize, &a, pE->fp.h.t.listeColonnes[x].size);

							break;
						}
						case CHAR:

							memcpy(pE->p[i].data + h*pE->fp.h.t.tailleElem + totalSize, q->tabValeurs[k], pE->fp.h.t.listeColonnes[x].size);

							break;
						case FLOAT:
						{
							char *ptr;

							float a;
							a = strtof(q->tabValeurs[k], &ptr);

							memcpy(pE->p[i].data + h*pE->fp.h.t.tailleElem + totalSize, &a, pE->fp.h.t.listeColonnes[x].size);

							break;
						}
						case DOUBLE:
						{
							char *ptr;

							double a;
							a = strtod(q->tabValeurs[k], &ptr);

							memcpy(pE->p[i].data + h*pE->fp.h.t.tailleElem + totalSize, &a, pE->fp.h.t.listeColonnes[x].size);

							break;
						}
						case DATE:
						{
							struct tm *date;
							date = malloc(sizeof(struct tm));

							char *val;
							val = malloc(sizeof(q->tabValeurs[k]));

							char *ptr;

							char *copy;
							copy = malloc(strlen(q->tabValeurs[k])+1);
							memcpy(copy, q->tabValeurs[k], strlen(q->tabValeurs[k])+1);

							//Jour
							val = strtok(copy, "/");
							if(val == NULL)
							{
								printf("Syntax error : invalid Date");
								return;
							}

							date->tm_mday = strtol(val, &ptr, 10);

							//Mois
							val = strtok(NULL, "/");
							if(val == NULL)
							{
								printf("Syntax error : invalid Date");
								return;
							}

							date->tm_mon = strtol(val, &ptr, 10);

							//Année
							val = strtok(NULL, "/");
							if(val == NULL)
							{
								printf("Syntax error : invalid Date");
								return;
							}

							date->tm_year = strtol(val, &ptr, 10);

							memcpy(pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, date, pE->fp.h.t.listeColonnes[x].size);

							totalSize += pE->fp.h.t.listeColonnes[i].size;

							free(date);
							free(copy);

							break;
						}
						}

					}

					//On indique que l'extension a subi une modification
					pE->altered = 1;

				}

				h++;

			}

		}

		fn = fn->next;

	}

}






void supprimerElementTableWhere(query *q)
{

	int nbExtensions;
	nbExtensions = chargerBlocs(q->nomTable);

	if(nbExtensions == 0)
		return;

	//Pointeur sur extension courante
	extension *pE;
	pE = malloc(sizeof(extension));


	int totalSize = 0;

	int verif = 1;

	int x = 0;

	int h;

	int nbElements;



	FNode *fn;
	fn = malloc(sizeof(FNode));

	fn = file->premier;

	for(int exten=0; exten<nbExtensions; exten++)
	{

		pE = &fn->e;

		//Pour tout les blocs de l'extension
		for(int i=0;i<7;i++) {

			h = 0;


			nbElements = pE->p[i].h.nbElements;

			//Pour tout les éléments du bloc
			for(int j=0;j<nbElements;j++)
			{
				verif = 1;

				while(pE->p[i].data[h * pE->fp.h.t.tailleElem] == (int)NULL) {
					h++;
				}

				//Vérifications des conditions
				for(int k=0;k<q->where.nbConditions && verif == 1;k++)
				{

					totalSize = 0;

					x = 0;

					while(memcmp(pE->fp.h.t.listeColonnes[x].nom, q->where.tabNomColonne[k], strlen(q->where.tabNomColonne[k])) != 0 && x < pE->fp.h.t.nbColonnes)
					{

						totalSize += pE->fp.h.t.listeColonnes[x].size;
						x++;
					}

					if(memcmp(pE->fp.h.t.listeColonnes[x].nom, q->where.tabNomColonne[k], strlen(q->where.tabNomColonne[k])) != 0)
					{

						printf("Colonne inexistante\n");
						return;
					}



					switch(pE->fp.h.t.listeColonnes[x].type)
					{
					case INT:
					{
						char *ptr;

						int z;
						z = strtol(q->where.tabValeur[k], &ptr, 10);

						int *y;
						y = malloc(sizeof(int));
						memcpy(y, pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, sizeof(int));


						//Conditions
						if (memcmp(q->where.tabOperateur[k], "=", strlen("=")+1) == 0)
						{

							if(z != *y)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], "!=", strlen("!=")+1) == 0)
						{

							if(z == *y)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], ">", strlen(">")+1) == 0)
						{
							if(*y <= z)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], "<", strlen("<")+1) == 0)
						{
							if(*y >= z)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], ">=", strlen(">=")+1) == 0)
						{
							if(*y < z)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], "<=", strlen("<=")+1) == 0)
						{
							if(*y > z)
							{
								verif = 0;
							}
						}

						break;
					}
					case CHAR:
						//Conditions
						if (memcmp(q->where.tabOperateur[k], "=", strlen("=")+1) == 0)
						{

							if(memcmp(pE->p[i].data + h*pE->fp.h.t.tailleElem + totalSize, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1) != 0)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], "!=", strlen("!=")+1) == 0)
						{

							if(memcmp(pE->p[i].data + h*pE->fp.h.t.tailleElem + totalSize, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1) == 0)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], ">", strlen(">")+1) == 0)
						{
							if(memcmp(pE->p[i].data + h*pE->fp.h.t.tailleElem + totalSize, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1) <= 0)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], "<", strlen("<")+1) == 0)
						{
							if(memcmp(pE->p[i].data + h*pE->fp.h.t.tailleElem + totalSize, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1) >= 0)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], ">=", strlen(">=")+1) == 0)
						{
							if(memcmp(pE->p[i].data + h*pE->fp.h.t.tailleElem + totalSize, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1) < 0)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], "<=", strlen("<=")+1) == 0)
						{
							if(memcmp(pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1) > 0)
							{
								verif = 0;
							}
						}

						break;
					case FLOAT:
					{
						char *ptr;

						float z;
						z = strtof(q->where.tabValeur[k], &ptr);

						float *y;
						y = malloc(sizeof(float));
						memcpy(y, pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, sizeof(float));


						//Conditions
						if (memcmp(q->where.tabOperateur[k], "=", strlen("=")+1) == 0)
						{

							if(z != *y)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], "!=", strlen("!=")+1) == 0)
						{

							if(z == *y)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], ">", strlen(">")+1) == 0)
						{
							if(*y <= z)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], "<", strlen("<")+1) == 0)
						{
							if(*y >= z)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], ">=", strlen(">=")+1) == 0)
						{
							if(*y < z)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], "<=", strlen("<=")+1) == 0)
						{
							if(*y > z)
							{
								verif = 0;
							}
						}

						break;
					}
					case DOUBLE:

					{
						char *ptr;

						double z;
						z = strtod(q->where.tabValeur[k], &ptr);

						double *y;
						y = malloc(sizeof(double));
						memcpy(y, pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, sizeof(double));


						//Conditions
						if (memcmp(q->where.tabOperateur[k], "=", strlen("=")+1) == 0)
						{

							if(z != *y)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], "!=", strlen("!=")+1) == 0)
						{

							if(z == *y)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], ">", strlen(">")+1) == 0)
						{
							if(*y <= z)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], "<", strlen("<")+1) == 0)
						{
							if(*y >= z)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], ">=", strlen(">=")+1) == 0)
						{
							if(*y < z)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], "<=", strlen("<=")+1) == 0)
						{
							if(*y > z)
							{
								verif = 0;
							}
						}

						break;
					}

					case DATE:
					{
						struct tm *newDate;
						newDate = malloc(sizeof(struct tm));

						char *val;
						val = malloc(sizeof(q->where.tabValeur[k]));

						char *ptr;

						char *copy;
						copy = malloc(strlen(q->where.tabValeur[k])+1);
						memcpy(copy, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1);

						//Jour
						val = strtok(copy, "/");
						if(val == NULL)
						{
							printf("Syntax error : invalid Date");
							return;
						}

						newDate->tm_mday = strtol(val, &ptr, 10);

						//Mois
						val = strtok(NULL, "/");
						if(val == NULL)
						{
							printf("Syntax error : invalid Date");
							return;
						}

						newDate->tm_mon = strtol(val, &ptr, 10);

						//Année
						val = strtok(NULL, "/");
						if(val == NULL)
						{
							printf("Syntax error : invalid Date");
							return;
						}

						newDate->tm_year = strtol(val, &ptr, 10);


						struct tm *date;
						date = malloc(sizeof(struct tm));
						memcpy(date, pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, sizeof(struct tm));

						newDate->tm_hour = newDate->tm_min = newDate->tm_sec = 0;
						date->tm_hour = date->tm_min = date->tm_sec = 0;

						double dif = difftime(mktime(date), mktime(newDate));


						//Conditions
						if (memcmp(q->where.tabOperateur[k], "=", strlen("=")+1) == 0)
						{

							if(dif != 0)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], "!=", strlen("!=")+1) == 0)
						{

							if(dif == 0)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], ">", strlen(">")+1) == 0)
						{
							if(dif <= 0)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], "<", strlen("<")+1) == 0)
						{
							if(dif >= 0)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], ">=", strlen(">=")+1) == 0)
						{
							if(dif < 0)
							{
								verif = 0;
							}
						}
						else if (memcmp(q->where.tabOperateur[k], "<=", strlen("<=")+1) == 0)
						{
							if(dif > 0)
							{
								verif = 0;
							}
						}


						free(newDate);
						free(date);

						free(copy);

						break;
					}
					}

					//Cas où OR est présent
					if(verif == 1 && k == q->where.nbConditions-2)
					{
						if(memcmp(q->where.tabDelim[k], "OR", strlen("OR")) == 0)
						{
							break;
						}
					}


					if(verif == 0 && k<q->where.nbConditions-1)
					{
						if(memcmp(q->where.tabDelim[k], "OR", strlen("OR")) == 0)
						{
							verif = 1;
						}
					}


				}

				//Si l'élément correspond aux conditions de la clause WHERE
				if(verif == 1)
				{
					pE->p[i].data[h * pE->fp.h.t.tailleElem] = 0;
					pE->p[i].h.nbElements--;
					h--;

					//On indique que l'extension a subi une modification
					pE->altered = 1;

				}

				h++;

			}



		}

		fn = fn->next;

	}



}




void selectionner(query *q)
{


	int nbExtensions;
	nbExtensions = chargerBlocs(q->nomTable);

	if(nbExtensions == 0)
	{
		printf("No table %s found\n", q->nomTable);
		return;
	}


	extension *pE = malloc(sizeof(extension));

	int totalSize;

	int verif = 1;

	int x;

	int h;

	FNode *fn;
	fn = malloc(sizeof(FNode));

	fn = file->premier;

	int selectAll = 0;

	int *tabNbExt;
	char *tabNomTable[20];

	int compteur = 0;

	int nbExtensionsTotal;


	if(q->nbJoin > 0)
	{

		*tabNomTable = malloc(sizeof(tabNomTable) * q->nbJoin+1);
		tabNomTable[0] = q->nomTable;

		tabNbExt = calloc(sizeof(int), q->nbJoin+2);
		tabNbExt[0] = nbExtensions;
		nbExtensionsTotal = nbExtensions;

		//Pour le cas où toutes les colonnes sont demandées avec le paramètre *
		if(q->nbColonnes == 1 && strncmp(q->tabColonne[0].nom, "*", strlen("*")+1) == 0)
		{

			fn = file->premier;
			pE = &fn->e;

			//Ajout des colonnes au tableau de colonnes
			for(int col=0;col<pE->fp.h.t.nbColonnes;col++)
			{
				char *tabCol = malloc(strlen(pE->fp.h.t.nom)+1 + strlen(pE->fp.h.t.listeColonnes[col].nom)+1 + 2);
				memcpy(tabCol, pE->fp.h.t.nom, strlen(pE->fp.h.t.nom)+1);

				strcat(tabCol, ".");
				strcat(tabCol, pE->fp.h.t.listeColonnes[col].nom);

				memcpy(q->tabColonne[col].nom, tabCol, strlen(tabCol)+1);
			}

			//Modification du nombre de colonnes
			q->nbColonnes = pE->fp.h.t.nbColonnes;

			selectAll = 1;
		}


		for(int tableJoin=0; tableJoin<q->nbJoin;tableJoin++)
		{
			nbExtensions = chargerBlocs(q->join[tableJoin].nomTable);

			if(nbExtensions == 0)
			{
				printf("Table inexistante\n");
				return;
			}

			fn = file->premier;
			pE = &fn->e;

			tabNomTable[tableJoin+1] = q->join[tableJoin].nomTable;

			tabNbExt[tableJoin+1] = nbExtensions;
			nbExtensionsTotal += nbExtensions;

			//Ajout des colonnes à la liste des colonnes à afficher
			if(selectAll)
			{

				//Ajout des colonnes au tableau de colonnes
				for(int col=0;col<pE->fp.h.t.nbColonnes;col++)
				{
					char *tabCol = malloc(strlen(pE->fp.h.t.nom)+1 + strlen(pE->fp.h.t.listeColonnes[col].nom)+1 + 2);
					memcpy(tabCol, pE->fp.h.t.nom, strlen(pE->fp.h.t.nom)+1);

					strcat(tabCol, ".");
					strcat(tabCol, pE->fp.h.t.listeColonnes[col].nom);

					memcpy(q->tabColonne[q->nbColonnes+col].nom, tabCol, strlen(tabCol)+1);
				}

				//Modification du nombre de colonnes
				q->nbColonnes += pE->fp.h.t.nbColonnes;
			}

		}

		joinTable(q, tabNbExt, tabNomTable);

	}



	else
	{
		fn = file->premier;

		//Pour le cas où toutes les colonnes sont demandées avec le paramètre *
		if(q->nbColonnes == 1 && strncmp(q->tabColonne[0].nom, "*", strlen("*")+1) == 0)
		{

			pE = &fn->e;

			//Ajout des colonnes au tableau de colonnes
			for(int k=0;k<pE->fp.h.t.nbColonnes;k++)
			{
				memcpy(q->tabColonne[k].nom, pE->fp.h.t.listeColonnes[k].nom, strlen(pE->fp.h.t.listeColonnes[k].nom));
				q->tabColonne[k].nom[strlen(pE->fp.h.t.listeColonnes[k].nom)] = '\0';
			}

			//Modification du nombre de colonnes
			q->nbColonnes = pE->fp.h.t.nbColonnes;

		}


		for(int exten=0; exten<nbExtensions; exten++)
		{

			pE = &fn->e;

			//Vérifications des colonnes demandées
			for(int k=0;k<q->nbColonnes;k++)
			{

				totalSize = 0;

				x = 0;

				while(memcmp(pE->fp.h.t.listeColonnes[x].nom, q->tabColonne[k].nom, strlen(q->tabColonne[k].nom)+1) != 0 && x < pE->fp.h.t.nbColonnes)
				{

					totalSize += pE->fp.h.t.listeColonnes[x].size;
					x++;
				}

				if(memcmp(pE->fp.h.t.listeColonnes[x].nom, q->tabColonne[k].nom, strlen(q->tabColonne[k].nom)+1) != 0)
				{

					printf("Colonne inexistante\n");
					return;
				}

				printf("%s\t\t\t", q->tabColonne[k].nom);

			}

			printf("\n");

			for(int i=0;i<q->nbColonnes;i++)
				printf("----------------------- ");

			printf("\n");

			//Pour tout les blocs de l'extension
			for(int i=0;i<7;i++)
			{

				h = 0;

				//Pour tout les éléments du bloc
				for(int j=0;j<pE->p[i].h.nbElements;j++)
				{

					while(pE->p[i].data[h * pE->fp.h.t.tailleElem] == (int)NULL)
					{
						h++;
					}


					verif = 1;

					//S'il y a une clause where
					if(q->where.nbConditions != 0)
					{


						//Vérifications des conditions
						for(int k=0;k<q->where.nbConditions && verif == 1;k++)
						{

							totalSize = 0;

							x = 0;

							while(memcmp(pE->fp.h.t.listeColonnes[x].nom, q->where.tabNomColonne[k], strlen(q->where.tabNomColonne[k])+1) != 0 && x < pE->fp.h.t.nbColonnes)
							{

								totalSize += pE->fp.h.t.listeColonnes[x].size;
								x++;
							}

							if(memcmp(pE->fp.h.t.listeColonnes[x].nom, q->where.tabNomColonne[k], strlen(q->where.tabNomColonne[k])+1) != 0)
							{

								printf("Colonne inexistante\n");
								return;
							}



							switch(pE->fp.h.t.listeColonnes[x].type)
							{
							case INT:
							{
								char *ptr;

								int z;
								z = strtol(q->where.tabValeur[k], &ptr, 10);

								int *y;
								y = malloc(sizeof(int));
								memcpy(y, pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, sizeof(int));


								//Conditions
								if (memcmp(q->where.tabOperateur[k], "=", strlen("=")+1) == 0)
								{

									if(z != *y)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], "!=", strlen("!=")+1) == 0)
								{

									if(z == *y)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], ">", strlen(">")+1) == 0)
								{
									if(*y <= z)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], "<", strlen("<")+1) == 0)
								{
									if(*y >= z)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], ">=", strlen(">=")+1) == 0)
								{
									if(*y < z)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], "<=", strlen("<=")+1) == 0)
								{
									if(*y > z)
									{
										verif = 0;
									}
								}

								break;
							}
							case CHAR:
								//Conditions
								if (memcmp(q->where.tabOperateur[k], "=", strlen("=")+1) == 0)
								{

									if(memcmp(pE->p[i].data + h*pE->fp.h.t.tailleElem + totalSize, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1) != 0)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], "!=", strlen("!=")+1) == 0)
								{

									if(memcmp(pE->p[i].data + h*pE->fp.h.t.tailleElem + totalSize, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1) == 0)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], ">", strlen(">")+1) == 0)
								{
									if(memcmp(pE->p[i].data + h*pE->fp.h.t.tailleElem + totalSize, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1) <= 0)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], "<", strlen("<")+1) == 0)
								{
									if(memcmp(pE->p[i].data + h*pE->fp.h.t.tailleElem + totalSize, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1) >= 0)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], ">=", strlen(">=")+1) == 0)
								{
									if(memcmp(pE->p[i].data + h*pE->fp.h.t.tailleElem + totalSize, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1) < 0)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], "<=", strlen("<=")+1) == 0)
								{
									if(memcmp(pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1) > 0)
									{
										verif = 0;
									}
								}

								break;
							case FLOAT:
							{
								char *ptr;

								float z;
								z = strtof(q->where.tabValeur[k], &ptr);

								float *y;
								y = malloc(sizeof(float));
								memcpy(y, pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, sizeof(float));


								//Conditions
								if (memcmp(q->where.tabOperateur[k], "=", strlen("=")+1) == 0)
								{

									if(z != *y)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], "!=", strlen("!=")+1) == 0)
								{

									if(z == *y)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], ">", strlen(">")+1) == 0)
								{
									if(*y <= z)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], "<", strlen("<")+1) == 0)
								{
									if(*y >= z)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], ">=", strlen(">=")+1) == 0)
								{
									if(*y < z)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], "<=", strlen("<=")+1) == 0)
								{
									if(*y > z)
									{
										verif = 0;
									}
								}

								break;
							}
							case DOUBLE:

							{
								char *ptr;

								double z;
								z = strtod(q->where.tabValeur[k], &ptr);

								double *y;
								y = malloc(sizeof(double));
								memcpy(y, pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, sizeof(double));


								//Conditions
								if (memcmp(q->where.tabOperateur[k], "=", strlen("=")+1) == 0)
								{

									if(z != *y)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], "!=", strlen("!=")+1) == 0)
								{

									if(z == *y)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], ">", strlen(">")+1) == 0)
								{
									if(*y <= z)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], "<", strlen("<")+1) == 0)
								{
									if(*y >= z)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], ">=", strlen(">=")+1) == 0)
								{
									if(*y < z)
									{
										verif = 0;
									}
								}
								else if (memcmp(q->where.tabOperateur[k], "<=", strlen("<=")+1) == 0)
								{
									if(*y > z)
									{
										verif = 0;
									}
								}

								break;
							}
							case DATE:
							{
								struct tm *newDate;
								newDate = malloc(sizeof(struct tm));

								char *val;
								val = malloc(strlen(q->where.tabValeur[k])+1);

								char *ptr;

								char *copy;
								copy = malloc(strlen(q->where.tabValeur[k])+1);
								memcpy(copy, q->where.tabValeur[k], strlen(q->where.tabValeur[k])+1);

								//Jour
								val = strtok(copy, "/");
								if(val == NULL)
								{
									printf("Syntax error : invalid Date");
									return;
								}

								newDate->tm_mday = strtol(val, &ptr, 10);

								//Mois
								val = strtok(NULL, "/");
								if(val == NULL)
								{
									printf("Syntax error : invalid Date");
									return;
								}

								newDate->tm_mon = strtol(val, &ptr, 10);

								//Année
								val = strtok(NULL, "/");
								if(val == NULL)
								{
									printf("Syntax error : invalid Date");
									return;
								}

								newDate->tm_year = strtol(val, &ptr, 10);

								struct tm *date;
								date = malloc(sizeof(struct tm));
								memcpy(date, pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, sizeof(struct tm));

								newDate->tm_hour = newDate->tm_min = newDate->tm_sec = 0;
								date->tm_hour = date->tm_min = date->tm_sec = 0;

								double dif = difftime(mktime(date), mktime(newDate));


								//Conditions
								if (memcmp(q->where.tabOperateur[k], "=", strlen("=")+1) == 0)
								{
									if(dif != 0)
										verif = 0;
								}
								else if (memcmp(q->where.tabOperateur[k], "!=", strlen("!=")+1) == 0)
								{
									if(dif == 0)
										verif = 0;
								}
								else if (memcmp(q->where.tabOperateur[k], ">", strlen(">")+1) == 0)
								{
									if(dif <= 0)
										verif = 0;
								}
								else if (memcmp(q->where.tabOperateur[k], "<", strlen("<")+1) == 0)
								{
									if(dif >= 0)
										verif = 0;
								}
								else if (memcmp(q->where.tabOperateur[k], ">=", strlen(">=")+1) == 0)
								{
									if(dif < 0)
										verif = 0;
								}
								else if (memcmp(q->where.tabOperateur[k], "<=", strlen("<=")+1) == 0)
								{
									if(dif > 0)
										verif = 0;
								}


								free(newDate);
								free(date);

								break;
							}

							}

							//Cas où OR est présent
							if(verif == 0 && k<q->where.nbConditions-1)
							{
								if(memcmp(q->where.tabDelim[k], "OR", strlen("OR")) == 0)
									verif = 1;
							}


						}
					}

					//Si l'élément correspond aux conditions de la clause WHERE
					if(verif == 1)
					{
						compteur++;
						//Vérifications des conditions
						for(int k=0;k<q->nbColonnes;k++)
						{

							totalSize = 0;

							x = 0;

							while(memcmp(pE->fp.h.t.listeColonnes[x].nom, q->tabColonne[k].nom, strlen(q->tabColonne[k].nom)+1) != 0 && x < pE->fp.h.t.nbColonnes)
							{

								totalSize += pE->fp.h.t.listeColonnes[x].size;
								x++;
							}

							switch(pE->fp.h.t.listeColonnes[x].type) {
							case INT:
							{
								int *y;
								y = malloc(sizeof(int));

								memcpy(y, pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, sizeof(int));

								printf("%d\t\t\t", *y);

								free(y);

								break;
							}
							case CHAR:
							{
								char *string;
								string = malloc(pE->fp.h.t.listeColonnes[x].size);

								memcpy(string, pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, pE->fp.h.t.listeColonnes[x].size);

								printf("%s\t\t\t", string);

								free(string);

								break;
							}
							case FLOAT:
							{
								float *y;
								y = malloc(sizeof(float));

								memcpy(y, pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, sizeof(float));

								printf("%f\t\t\t", *y);

								free(y);

								break;
							}

							case DOUBLE:
							{
								double *y;
								y = malloc(sizeof(double));

								memcpy(y, pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, sizeof(double));

								printf("%f\t\t\t", *y);

								free(y);

								break;
							}
							case DATE:
							{
								struct tm *date;
								date = malloc(sizeof(struct tm));

								memcpy(date, pE->p[i].data + h * pE->fp.h.t.tailleElem + totalSize, sizeof(struct tm));

								printf("%d/%d/%d\t\t\t", date->tm_mday, date->tm_mon, date->tm_year);

								free(date);

								break;
							}

							}

						}

						printf("\n");


					}

					h++;

				}



			}

			fn = fn->next;

		}

		printf("\nNumber of elements in table : %d\n", compteur);
	}
	//Libération de la mémoire allouée pour le pointeur sur l'extension
	//free(pE);



}




void afficherValeur(extension *pE, colonne c, void *row, int decCol)
{

	switch(c.type) {
	case INT:
	{
		int *y;
		y = malloc(sizeof(int));

		memcpy(y, row + decCol, sizeof(int));

		printf("%d\t\t\t", *y);

		free(y);

		break;
	}
	case CHAR:
	{
		char *string;
		string = malloc(c.size);

		memcpy(string, row + decCol, c.size);

		printf("%s\t\t\t", string);

		free(string);

		break;
	}
	case FLOAT:
	{
		float *y;
		y = malloc(sizeof(float));

		memcpy(y, row + decCol, sizeof(float));

		printf("%f\t\t\t", *y);

		free(y);

		break;
	}
	case DOUBLE:
	{
		double *y;
		y = malloc(sizeof(double));

		memcpy(y, row + decCol, sizeof(double));

		printf("%f\t\t\t", *y);

		free(y);

		break;
	}

	case DATE:
	{
		struct tm *date;
		date = malloc(sizeof(struct tm));

		memcpy(date, row + decCol, sizeof(struct tm));

		printf("%d/%d/%d\t\t\t", date->tm_mday, date->tm_mon, date->tm_year);

		free(date);

		break;
	}


	}


}
