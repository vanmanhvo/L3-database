/*
 * joinAlgorithms.c
 *
 *  Created on: 17 août 2016
 *      Author: Andy
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

void joinTable(query *q, int tabNbExt[], char *tabNomTable[])
{

	FNode *fnNew = malloc(sizeof(FNode));

	FNode *fn1;
	FNode *fn2;

	int nbExt1;
	int nbExt2;

	char *nomTableJoin;

	int numTableJ;
	int dec;

	numTableJ = q->nbJoin;
	dec = 0;


	fn1 = file->premier;

	for(int i=q->nbJoin+1; i>0; i--)
	{
		for(int j=0; j<tabNbExt[i]; j++)
			fn1 = fn1->next;
	}


	nbExt1 = tabNbExt[0];

	for(int i=0; i<q->nbJoin; i++)
	{
		nomTableJoin = q->join[i].nomTable;

		numTableJ = q->nbJoin;

		dec = 0;

		while(memcmp(nomTableJoin, tabNomTable[numTableJ], strlen(tabNomTable[numTableJ])+1) != 0)
		{
			dec += tabNbExt[numTableJ];
			numTableJ--;
		}

		fn2 = file->premier;
		for(int i=0; i<dec; i++)
			fn2 = fn2->next;

		nbExt2 = tabNbExt[numTableJ];

		if(memcmp(fn1->e.fp.h.t.nom, "joinTable", strlen("joinTable")+1) == 0)
		{
			int compteur = 0;

			while(fn1->next != NULL)
				fn1 = fn1->next;
			do {
				compteur++;
				if(fn1->prev != NULL)
					fn1 = fn1->prev;
			} while(fn1->prev != NULL);
			nbExt1 = compteur;
		}

		nestedLoop(&q->join[i], fn1, fn2, nbExt1, nbExt2, fnNew);
		//mergeJoin(&q->join[i], fn1, fn2, nbExt1, nbExt2, fnNew);
		//hashJoin(&q->join[i], fn1, fn2, nbExt1, nbExt2, fnNew);

		fn1 = fnNew;
		fnNew = malloc(sizeof(FNode));
	}

	printJoinResult(fn1, q);

}



void nestedLoop(clauseJoin *cJ, FNode *fn, FNode *fnJ, int nbExt, int nbExtJ, FNode *fnNew)
{

	if(memcmp(fn->e.fp.h.t.nom, "joinTable", strlen("joinTable")+1) == 0)
	{
		while(fn->prev != NULL)
			fn = fn->prev;
	}

	//VARIABLES
	FNode *fnJbis = malloc(sizeof(FNode));

	extension *pE = malloc(sizeof(extension));
	extension *pEJ = malloc(sizeof(extension));

	int rowId, rowIdJ;
	int verif;
	int numCol, numColJ;

	void *row, *rowJ;

	//CREATION DE LA TABLE JOIN
	pE = &fn->e;
	pEJ = &fnJ->e;
	newJoinTable(fnNew, pE, pEJ);

	//Pour toutes les extensions de la table d'origine
	for(int ext=0; ext<nbExt && fn != NULL; ext++)
	{

		pE = &fn->e;

		//Pour tout les blocs de l'extension
		for(int bloc=0; bloc<7; bloc++)
		{

			rowId = 0;


			for(int nbElem = 0; nbElem < pE->p[bloc].h.nbElements;nbElem++)
			{

				while(pE->p[bloc].data[rowId * pE->fp.h.t.tailleElem] == (int)NULL)
				{
					rowId++;
				}


				//On compare cet élément à tout les éléments de la table de JOIN
				//Pour toutes les extensions de la table JOIN
				fnJbis = fn;

				for(int extJ = 0; extJ<nbExtJ; extJ++)
				{
					pEJ = &fnJ->e;

					//Pour tout les blocs de l'extension
					for(int blocJ=0; blocJ<7; blocJ++)
					{

						rowIdJ = 0;


						for(int nbElemJ = 0; nbElemJ < pEJ->p[blocJ].h.nbElements; nbElemJ++)
						{

							while(pEJ->p[bloc].data[rowIdJ * pEJ->fp.h.t.tailleElem] == (int)NULL)
							{
								rowIdJ++;
							}

							//Verif
							verif = 1;

							//On vérifie les conditions du join
							for(int condi=0; condi < cJ->on.nbConditions && verif == 1;condi++)
							{
								char *copy1 = malloc(strlen(cJ->on.tabNomColonne[condi])+1);
								memcpy(copy1, cJ->on.tabNomColonne[condi], strlen(cJ->on.tabNomColonne[condi])+1);

								char *copy2 = malloc(strlen(cJ->on.tabValeur[condi])+1);
								memcpy(copy2, cJ->on.tabValeur[condi], strlen(cJ->on.tabValeur[condi])+1);

								char *nomTable1 = strtok(copy1, ".");
								char *nomColonne1 = strtok(NULL, "");

								char *nomTable2 = NULL;
								char *nomColonne2 = NULL;

								if(memcmp(pE->fp.h.t.nom, "joinTable", strlen("joinTable")+1) == 0)
								{
									if(memcmp(nomTable1, pEJ->fp.h.t.nom, strlen(pEJ->fp.h.t.nom)+1) == 0)
									{
										nomTable2 = nomTable1;
										nomColonne2 = nomColonne1;

										nomTable1 = pE->fp.h.t.nom;
										nomColonne1 = cJ->on.tabValeur[condi];
									}
									else
									{
										nomTable2 = strtok(copy2, ".");
										nomColonne2 = strtok(NULL, "");

										if(memcmp(nomTable2, pEJ->fp.h.t.nom, strlen(pEJ->fp.h.t.nom)+1) == 0)
										{
											nomTable1 = pE->fp.h.t.nom;
											nomColonne1 = cJ->on.tabNomColonne[condi];
										}
										else
										{
											printf("Table inexistante\n");
											return;
										}
									}

								}


								else if(memcmp(nomTable1, pE->fp.h.t.nom, strlen(pE->fp.h.t.nom)+1) == 0)
								{
									nomTable2 = strtok(copy2, ".");
									nomColonne2 = strtok(NULL, ".");

									if(memcmp(nomTable2, pEJ->fp.h.t.nom, strlen(pEJ->fp.h.t.nom)+1) != 0)
									{
										printf("Table inexistante\n");
										return;
									}
								}

								else if(memcmp(nomTable1, pEJ->fp.h.t.nom, strlen(pEJ->fp.h.t.nom)+1) == 0)
								{
									nomTable2 = nomTable1;
									nomColonne2 = nomColonne1;

									nomTable1 = strtok(copy2, ".");
									nomColonne1 = strtok(NULL, ".");
									if(memcmp(nomTable1, pE->fp.h.t.nom, strlen(pE->fp.h.t.nom)+1) != 0)
									{
										printf("Table inexistante\n");
										return;
									}
								}


								int decCol1 = 0;
								int decCol2 = 0;

								//Vérifications de la première colonne
								numCol = 0;

								while(memcmp(pE->fp.h.t.listeColonnes[numCol].nom, nomColonne1, strlen(nomColonne1)+1) != 0 && numCol < pE->fp.h.t.nbColonnes)
								{
									decCol1 += pE->fp.h.t.listeColonnes[numCol].size;
									numCol++;
								}

								if(memcmp(pE->fp.h.t.listeColonnes[numCol].nom, nomColonne1, strlen(nomColonne1)+1) != 0)
								{
									printf("Colonne inexistante\n");
									return;
								}

								//Vérification de la deuxième colonne
								numColJ = 0;

								while(memcmp(pEJ->fp.h.t.listeColonnes[numColJ].nom, nomColonne2, strlen(nomColonne2)+1) != 0 && numColJ < pEJ->fp.h.t.nbColonnes)
								{
									decCol2 += pEJ->fp.h.t.listeColonnes[numColJ].size;
									numColJ++;
								}


								if(memcmp(pEJ->fp.h.t.listeColonnes[numColJ].nom, nomColonne2, strlen(nomColonne2)+1) != 0)
								{
									printf("Colonne inexistante\n");
									return;
								}

								//Vérification de la cohérence des types de données
								if(pE->fp.h.t.listeColonnes[numCol].type != pEJ->fp.h.t.listeColonnes[numColJ].type)
								{
									printf("Types incompatibles\n");
									return;
								}

								row = pE->p[bloc].data + rowId * pE->fp.h.t.tailleElem;
								rowJ = pEJ->p[blocJ].data + rowIdJ * pEJ->fp.h.t.tailleElem;

								//Comparaison des deux éléments
								verif = comparerElements(pE->fp.h.t.listeColonnes[numCol].type, pE->fp.h.t.listeColonnes[numCol].size, pE, pEJ, row, rowJ, decCol1, decCol2);


								free(copy1);
								free(copy2);

							}

							//Si l'élément correspond aux conditions des clauses JOIN et WHERE
							if(verif == 1)
							{
								insertIntoJoinTable(fnNew, pE, pEJ, row, rowJ);
							}
							rowIdJ++;
						}
					}
					fnJbis = fnJbis->next;
				}
				rowId++;
			}
		}
		fn = fn->next;
	}
	free(fn);
}



void mergeJoin(clauseJoin *cJ, FNode *fn, FNode *fnJ, int nbExt, int nbExtJ, FNode *fnNew)
{

	if(memcmp(fn->e.fp.h.t.nom, "joinTable", strlen("joinTable")+1) == 0)
	{
		while(fn->prev != NULL)
			fn = fn->prev;
	}

	//VARIABLES
	extension *pE = malloc(sizeof(extension));
	extension *pEJ = malloc(sizeof(extension));

	int nbElem, nbElemJoin;

	elemT *tab = NULL;
	elemT *tabJoin = NULL;

	TYPE *t = malloc(sizeof(TYPE));


	//CREATION DE LA TABLE JOIN
	pE = &fn->e;
	pEJ = &fnJ->e;
	newJoinTable(fnNew, pE, pEJ);

	char *nomCol;
	char *nomColJoin;

	char *copy1 = malloc(strlen(cJ->on.tabNomColonne[0])+1);
	memcpy(copy1, cJ->on.tabNomColonne[0], strlen(cJ->on.tabNomColonne[0])+1);

	char *copy2 = malloc(strlen(cJ->on.tabValeur[0])+1);
	memcpy(copy2, cJ->on.tabValeur[0], strlen(cJ->on.tabValeur[0])+1);

	char *nomTable1 = strtok(copy1, ".");
	char *nomColonne1 = strtok(NULL, "");

	char *nomTable2 = NULL;
	char *nomColonne2 = NULL;

	if(memcmp(pE->fp.h.t.nom, "joinTable", strlen("joinTable")+1) == 0)
	{
		if(memcmp(nomTable1, pEJ->fp.h.t.nom, strlen(pEJ->fp.h.t.nom)+1) == 0)
		{
			nomTable2 = nomTable1;
			nomColonne2 = nomColonne1;

			nomTable1 = pE->fp.h.t.nom;
			nomColonne1 = cJ->on.tabValeur[0];
		}
		else
		{
			nomTable2 = strtok(copy2, ".");
			nomColonne2 = strtok(NULL, "");

			if(memcmp(nomTable2, pEJ->fp.h.t.nom, strlen(pEJ->fp.h.t.nom)+1) == 0)
			{
				nomTable1 = pE->fp.h.t.nom;
				nomColonne1 = cJ->on.tabNomColonne[0];
			}
			else
			{
				printf("Table inexistante\n");
				return;
			}
		}

	}

	else if(memcmp(nomTable1, pE->fp.h.t.nom, strlen(pE->fp.h.t.nom)+1) == 0)
	{
		nomTable2 = strtok(copy2, ".");
		nomColonne2 = strtok(NULL, ".");

		if(memcmp(nomTable2, pEJ->fp.h.t.nom, strlen(pEJ->fp.h.t.nom)+1) != 0)
		{
			printf("Table inexistante\n");
			return;
		}
	}

	else if(memcmp(nomTable1, pEJ->fp.h.t.nom, strlen(pEJ->fp.h.t.nom)+1) == 0)
	{
		nomTable2 = nomTable1;
		nomColonne2 = nomColonne1;

		nomTable1 = strtok(copy2, ".");
		nomColonne1 = strtok(NULL, ".");
		if(memcmp(nomTable1, pE->fp.h.t.nom, strlen(pE->fp.h.t.nom)+1) != 0)
		{
			printf("Table inexistante\n");
			return;
		}
	}

	nomCol = nomColonne1;
	nomColJoin = nomColonne2;

	//Tri des tableaux
	tab = tri(fn, nomCol, tab, &nbElem, nbExt, t);
	tabJoin = tri(fnJ, nomColJoin, tabJoin, &nbElemJoin, nbExtJ, t);


	//MERGE
	int elem=0, elemJoin=0;

	switch(*t)
	{
	case INT:
	{
		int *val1 = tab[elem].valeur;
		int *val2 = tabJoin[elemJoin].valeur;

		while(elem<nbElem && elemJoin<nbElemJoin)
		{

			if(*val1 > *val2)
			{
				elemJoin++;
				if(elemJoin<nbElemJoin)
					val2 = tabJoin[elemJoin].valeur;
			}

			else if(*val1 < *val2)
			{
				elem++;
				if(elem<nbElem)
					val1 = tab[elem].valeur;
			}

			else
			{
				//Ajouter les valeurs à la table temporaire


				insertIntoJoinTable(fnNew, pE, pEJ, tab[elem].row, tabJoin[elemJoin].row);

				if(elemJoin+1<nbElemJoin)
				{
					val2 = tabJoin[elemJoin+1].valeur;
					if(*val1 == *val2)
					{
						elemJoin++;
					}
					else
						elem++;
				}

				else{
					elem++;
				}



			}

			if(elem<nbElem)
				val1 = tab[elem].valeur;
			if(elemJoin<nbElemJoin)
				val2 = tabJoin[elemJoin].valeur;

		}

		break;
	}

	case CHAR:
	{
		char *val1 = tab[elem].valeur;
		char *val2 = tabJoin[elemJoin].valeur;

		while(elem<nbElem && elemJoin<nbElemJoin)
		{

			if(memcmp(val1, val2, strlen(val1)+1) > 0)
			{
				elemJoin++;
				if(elemJoin<nbElemJoin)
					val2 = tabJoin[elemJoin].valeur;
			}

			else if(memcmp(val1, val2, strlen(val1)+1) < 0)
			{
				elem++;
				if(elem<nbElem)
					val1 = tab[elem].valeur;
			}

			else
			{
				//Ajouter les valeurs à la table temporaire


				insertIntoJoinTable(fnNew, pE, pEJ, tab[elem].row, tabJoin[elemJoin].row);

				if(elemJoin+1<nbElemJoin)
				{
					val2 = tabJoin[elemJoin+1].valeur;
					if(memcmp(val1, val2, strlen(val1)+1) == 0)
					{
						elemJoin++;
					}
					else
						elem++;
				}

				else{
					elem++;
				}



			}

			if(elem<nbElem)
				val1 = tab[elem].valeur;
			if(elemJoin<nbElemJoin)
				val2 = tabJoin[elemJoin].valeur;

		}

		break;
	}
	case FLOAT:
	{
		float *val1 = tab[elem].valeur;
		float *val2 = tabJoin[elemJoin].valeur;

		while(elem<nbElem && elemJoin<nbElemJoin)
		{

			if(*val1 > *val2)
			{
				elemJoin++;
				if(elemJoin<nbElemJoin)
					val2 = tabJoin[elemJoin].valeur;
			}

			else if(*val1 < *val2)
			{
				elem++;
				if(elem<nbElem)
					val1 = tab[elem].valeur;
			}

			else
			{
				//Ajouter les valeurs à la table temporaire


				insertIntoJoinTable(fnNew, pE, pEJ, tab[elem].row, tabJoin[elemJoin].row);

				if(elemJoin+1<nbElemJoin)
				{
					val2 = tabJoin[elemJoin+1].valeur;
					if(*val1 == *val2)
					{
						elemJoin++;
					}
					else
						elem++;
				}

				else{
					elem++;
				}

			}

			if(elem<nbElem)
				val1 = tab[elem].valeur;
			if(elemJoin<nbElemJoin)
				val2 = tabJoin[elemJoin].valeur;

		}

		break;
	}

	case DOUBLE:
	{
		double *val1 = tab[elem].valeur;
		double *val2 = tabJoin[elemJoin].valeur;

		while(elem<nbElem && elemJoin<nbElemJoin)
		{

			if(*val1 > *val2)
			{
				elemJoin++;
				if(elemJoin<nbElemJoin)
					val2 = tabJoin[elemJoin].valeur;
			}

			else if(*val1 < *val2)
			{
				elem++;
				if(elem<nbElem)
					val1 = tab[elem].valeur;
			}

			else
			{
				//Ajouter les valeurs à la table temporaire

				insertIntoJoinTable(fnNew, pE, pEJ,tab[elem].row, tabJoin[elemJoin].row);

				if(elemJoin+1<nbElemJoin)
				{
					val2 = tabJoin[elemJoin+1].valeur;
					if(*val1 == *val2)
					{
						elemJoin++;
					}
					else
						elem++;
				}

				else{
					elem++;
				}



			}

			if(elem<nbElem)
				val1 = tab[elem].valeur;
			if(elemJoin<nbElemJoin)
				val2 = tabJoin[elemJoin].valeur;

		}

		break;
	}

	case DATE:
	{
		struct tm *val1 = tab[elem].valeur;
		struct tm *val2 = tabJoin[elemJoin].valeur;

		while(elem<nbElem && elemJoin<nbElemJoin)
		{

			if(difftime(mktime(val1), mktime(val2)) > 0)
			{
				elemJoin++;
				if(elemJoin<nbElemJoin)
					val2 = tabJoin[elemJoin].valeur;
			}

			else if(difftime(mktime(val1), mktime(val2)) < 0)
			{
				elem++;
				if(elem<nbElem)
					val1 = tab[elem].valeur;
			}

			else
			{
				//Ajouter les valeurs à la table temporaire

				insertIntoJoinTable(fnNew, pE, pEJ,tab[elem].row, tabJoin[elemJoin].row);

				if(elemJoin+1<nbElemJoin)
				{
					val2 = tabJoin[elemJoin+1].valeur;
					if(difftime(mktime(val1), mktime(val2)) == 0)
					{
						elemJoin++;
					}
					else
						elem++;
				}

				else{
					elem++;
				}



			}

			if(elem<nbElem)
				val1 = tab[elem].valeur;
			if(elemJoin<nbElemJoin)
				val2 = tabJoin[elemJoin].valeur;

		}

		break;
	}
	}
}


void hashJoin(clauseJoin *cJ, FNode *fn, FNode *fnJ, int nbExt, int nbExtJ, FNode *fnNew)
{

	//1ère étape : Estimer la table la plus petite
	FNode *fnBuildInput, *fnProbeInput;



	int nbExtBuild, nbExtProbe;
	int rowId;
	int decColB = 0, numColB = 0;
	int decColP = 0, numColP = 0;
	int h_id;

	void *row;
	void *row2;

	extension *pEBuild = malloc(sizeof(extension));
	extension *pEProbe = malloc(sizeof(extension));


	if(nbExt/fn->e.fp.h.t.tailleElem < nbExtJ/fnJ->e.fp.h.t.tailleElem)
	{
		fnBuildInput = fn;
		fnProbeInput = fnJ;
		nbExtBuild = nbExt;
		nbExtProbe = nbExtJ;
	}
	else
	{
		fnBuildInput = fnJ;
		fnProbeInput = fn;
		nbExtBuild = nbExtJ;
		nbExtProbe = nbExt;
	}

	//CREATION DE LA TABLE JOIN
	pEBuild = &fnBuildInput->e;
	pEProbe = &fnProbeInput->e;
	newJoinTable(fnNew, pEBuild, pEProbe);

	char *nomColBuild;
	char *nomColProbe;

	char *copy1 = malloc(strlen(cJ->on.tabNomColonne[0])+1);
	memcpy(copy1, cJ->on.tabNomColonne[0], strlen(cJ->on.tabNomColonne[0])+1);

	char *copy2 = malloc(strlen(cJ->on.tabValeur[0])+1);
	memcpy(copy2, cJ->on.tabValeur[0], strlen(cJ->on.tabValeur[0])+1);

	char *nomTableBuild = strtok(copy1, ".");
	char *nomColonneBuild = strtok(NULL, "");

	char *nomTableProbe = NULL;
	char *nomColonneProbe = NULL;


	if(memcmp(pEBuild->fp.h.t.nom, "joinTable", strlen("joinTable")+1) == 0)
	{
		if(memcmp(nomTableBuild, pEProbe->fp.h.t.nom, strlen(pEProbe->fp.h.t.nom)+1) == 0)
		{
			nomTableProbe = nomTableBuild;
			nomColonneProbe = nomColonneBuild;

			nomTableBuild = pEBuild->fp.h.t.nom;
			nomColonneBuild = cJ->on.tabValeur[0];
		}
		else
		{
			nomTableProbe = strtok(copy2, ".");
			nomColonneProbe = strtok(NULL, "");

			if(memcmp(nomTableProbe, pEProbe->fp.h.t.nom, strlen(pEProbe->fp.h.t.nom)+1) == 0)
			{
				nomTableBuild = pEBuild->fp.h.t.nom;
				nomColonneBuild = cJ->on.tabNomColonne[0];
			}
			else
			{
				printf("Table inexistante\n");
				return;
			}
		}

	}

	else if(memcmp(pEProbe->fp.h.t.nom, "joinTable", strlen("joinTable")+1) == 0)
	{

		if(memcmp(nomTableBuild, pEBuild->fp.h.t.nom, strlen(pEBuild->fp.h.t.nom)+1) == 0)
		{

			nomTableProbe = pEProbe->fp.h.t.nom;
			nomColonneProbe = cJ->on.tabValeur[0];

			nomTableBuild = pEBuild->fp.h.t.nom;
			nomColonneBuild = cJ->on.tabNomColonne[0];
		}
		else
		{
			nomTableBuild = strtok(copy2, ".");
			nomColonneBuild = strtok(NULL, "");

			if(memcmp(nomTableBuild, pEBuild->fp.h.t.nom, strlen(pEBuild->fp.h.t.nom)+1) == 0)
			{
				nomTableProbe = pEProbe->fp.h.t.nom;
				nomColonneProbe = cJ->on.tabNomColonne[0];
			}
			else
			{
				printf("Table inexistante\n");
				return;
			}
		}

	}

	else if(memcmp(nomTableBuild, pEBuild->fp.h.t.nom, strlen(pEBuild->fp.h.t.nom)+1) == 0)
	{
		nomTableProbe = strtok(copy2, ".");
		nomColonneProbe = strtok(NULL, ".");

		if(memcmp(nomTableProbe, pEProbe->fp.h.t.nom, strlen(pEProbe->fp.h.t.nom)+1) != 0)
		{
			printf("Table inexistante\n");
			return;
		}
	}

	else if(memcmp(nomTableBuild, pEProbe->fp.h.t.nom, strlen(pEProbe->fp.h.t.nom)+1) == 0)
	{
		nomTableProbe = nomTableBuild;
		nomColonneProbe = nomColonneBuild;

		nomTableBuild = strtok(copy2, ".");
		nomColonneBuild = strtok(NULL, ".");
		if(memcmp(nomTableBuild, pEBuild->fp.h.t.nom, strlen(pEBuild->fp.h.t.nom)+1) != 0)
		{
			printf("Table inexistante\n");
			return;
		}
	}

	nomColBuild = nomColonneBuild;
	nomColProbe = nomColonneProbe;

	decColB = 0;

	for(numColB=0; numColB<pEBuild->fp.h.t.nbColonnes; numColB++)
	{
		if(memcmp(nomColBuild, pEBuild->fp.h.t.listeColonnes[numColB].nom, strlen(pEBuild->fp.h.t.listeColonnes[numColB].nom)+1) == 0)
			break;
		decColB += pEBuild->fp.h.t.listeColonnes[numColB].size;
	}


	if(memcmp(nomColBuild, pEBuild->fp.h.t.listeColonnes[numColB].nom, strlen(pEBuild->fp.h.t.listeColonnes[numColB].nom)+1) != 0)
	{
		printf("Unknown column : %s\n", nomColBuild);
		return;
	}

	//2ème étape : "Hasher" la table la plus petite
	hashTable *hT = creerHashTable(100);

	for(int ext=0; ext<nbExtBuild; ext++)
	{
		pEBuild = &fnBuildInput->e;

		for(int bloc=0; bloc<7; bloc++)
		{

			rowId = 0;

			for(int elem=0; elem<pEBuild->p[bloc].h.nbElements; elem++)
			{

				while(pEBuild->p[bloc].data[rowId * pEBuild->fp.h.t.tailleElem] == (int)NULL)
				{
					rowId++;
				}

				row = pEBuild->p[bloc].data + rowId * pEBuild->fp.h.t.tailleElem;

				addToHashTable(hT, pEBuild, row, numColB, decColB);

				rowId++;
			}

		}

		if(fnBuildInput->next != NULL)
			fnBuildInput = fnBuildInput->next;
	}

	hElem *hE;

	decColP = 0;

	//3ème étape : Scanner l'autre table
	for(numColP=0; numColP<pEProbe->fp.h.t.nbColonnes; numColP++)
	{

		if(memcmp(nomColProbe, pEProbe->fp.h.t.listeColonnes[numColP].nom, strlen(pEProbe->fp.h.t.listeColonnes[numColP].nom)+1) == 0)
			break;
		decColP += pEProbe->fp.h.t.listeColonnes[numColP].size;
	}

	if(memcmp(nomColProbe, pEProbe->fp.h.t.listeColonnes[numColP].nom, strlen(pEProbe->fp.h.t.listeColonnes[numColP].nom)+1) != 0)
	{
		printf("Unknown column : %s\n", nomColProbe);
		return;
	}


	for(int ext=0; ext<nbExtProbe; ext++)
	{
		pEProbe = &fnProbeInput->e;

		for(int bloc=0; bloc<7; bloc++)
		{

			rowId = 0;

			for(int elem=0; elem<pEProbe->p[bloc].h.nbElements; elem++)
			{

				while(pEProbe->p[bloc].data[rowId * pEProbe->fp.h.t.tailleElem] == (int)NULL)
				{
					rowId++;
				}


				h_id = hashValeur(pEProbe->p[bloc].data + rowId *pEProbe->fp.h.t.tailleElem + decColP, pEProbe->fp.h.t.listeColonnes[numColP].type, pEProbe->fp.h.t.listeColonnes[numColP].size) % hT->capacite;
				int verif = 0;

				if(hT->array[h_id] != NULL)
				{

					hE = hT->array[h_id];
					while(hE != NULL)
					{

						verif = 0;

						row = hE->e.row;
						row2 = pEProbe->p[bloc].data + rowId *pEProbe->fp.h.t.tailleElem;

						verif = comparerElements(pEBuild->fp.h.t.listeColonnes[numColB].type, pEProbe->fp.h.t.listeColonnes[numColP].size, pEBuild, pEProbe, row, row2, decColB, decColP);

						if(verif == 1)
						{
							insertIntoJoinTable(fnNew, pEBuild, pEProbe, row, row2);
						}
						hE = hE->next;
					}
				}


				rowId++;
			}

		}

		if(fnProbeInput->next != NULL)
			fnProbeInput = fnProbeInput->next;
	}

}


hashTable* creerHashTable(int capacite)
{
	hashTable* hT = malloc(sizeof(hashTable));
	hT->capacite = capacite;

	hT->array = malloc(hT->capacite * sizeof(hElem));

	for(int i=0; i<hT->capacite; i++)
	{
		hT->array[i] = NULL;
	}

	return hT;
}


int hashValeur(void *valeur, TYPE t, size_t taille)
{
	int hashValue = 0;

	switch(t)
	{
	case INT:
	{
		int *x = malloc(sizeof(int));
		memcpy(x, valeur, sizeof(int));
		hashValue = *x;

		free(x);

		break;
	}
	case CHAR:
	{
		char *x = malloc(taille);
		memcpy(x, valeur, taille);
		hashValue = hashString(x);

		free(x);

		break;
	}
	case FLOAT:
	{
		break;
	}
	case DOUBLE:
	{
		break;
	}
	case DATE:
	{
		break;
	}
	}

	return hashValue;
}

void addToHashTable(hashTable *hT, extension *pE, void *row, int numCol, int decCol)
{
	int h_id = 0;

	hElem *hE = malloc(sizeof(hElem));

	hE->next = NULL;

	elemT e;
	e.row = row;

	e.valeur = malloc(pE->fp.h.t.listeColonnes[numCol].size);
	memcpy(e.valeur, row + decCol, pE->fp.h.t.listeColonnes[numCol].size);


	h_id = hashValeur(e.valeur, pE->fp.h.t.listeColonnes[numCol].type, pE->fp.h.t.listeColonnes[numCol].size) % hT->capacite;

	hE->e = e;

	if(hT->array[h_id] == NULL)
		hT->array[h_id] = hE;
	else
	{
		hE->next = hT->array[h_id];
		hT->array[h_id] = hE;
	}

}

unsigned long hashString(char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

elemT* tri(FNode *fn, char *nomCol, elemT *tabElem, int *nbElem, int nbExt, TYPE *t)
{

	FNode *fnBis = malloc(sizeof(FNode));

	extension *pE = malloc(sizeof(extension));

	colonne *c = malloc(sizeof(colonne));

	int nbRow; //Nombre de lignes de données
	int dec = 0; //Décalage de bytes pour arriver à la valeur demandée dans une ligne de données
	int rowId;
	int elemNum = 0;

	fnBis = fn;

	nbRow = 0;

	for(int ext=0; ext<nbExt && fnBis != NULL; ext++)
	{
		pE = &fnBis->e;

		for(int bloc=0; bloc<7; bloc++)
			nbRow += pE->p[bloc].h.nbElements;

		fnBis = fnBis->next;
	}

	if(nbRow <= 0)
	{
		printf("No elements to sort\n");
		return NULL;
	}


	//Allocation de la mémoire pour le tableau d'éléments
	tabElem = malloc(sizeof(elemTri) * nbRow);


	//On trouve le décalage dans la ligne de donnée de la valeur voulue
	for(int col=0; col < pE->fp.h.t.nbColonnes; col++)
	{
		if(memcmp(nomCol, pE->fp.h.t.listeColonnes[col].nom, strlen(pE->fp.h.t.listeColonnes[col].nom+1)) == 0)
		{
			c = &pE->fp.h.t.listeColonnes[col];
			*t = c->type;
			break;
		}

		dec += pE->fp.h.t.listeColonnes[col].size;
	}

	//On stocke tout les éléments dans le tableau
	if(memcmp(fn->e.fp.h.t.nom, "joinTable", strlen("joinTable")+1) == 0)
	{
		while(fn->prev != NULL)
			fn = fn->prev;
	}

	for(int ext=0; ext<nbExt && fn != NULL; ext++)
	{

		pE = &fn->e;

		for(int bloc=0; bloc<7; bloc++)
		{

			rowId=0;

			for(int elem=0; elem<pE->p[bloc].h.nbElements; elem++)
			{
				while(pE->p[bloc].data[rowId * pE->fp.h.t.tailleElem] == (int)NULL)
				{
					rowId++;
				}

				//tabElem[elemNum].row = malloc(sizeof(pE->fp.h.t.tailleElem));
				tabElem[elemNum].row = pE->p[bloc].data + rowId * pE->fp.h.t.tailleElem;

				tabElem[elemNum].valeur = malloc(sizeof(c->size));
				memcpy(tabElem[elemNum].valeur, pE->p[bloc].data + rowId * pE->fp.h.t.tailleElem + dec, c->size);

				elemNum++;

				rowId++;
			}
		}

		fn = fn->next;

	}

	//On tri le tableau
	quickSort(tabElem, 0, elemNum-1, c);

	*nbElem = elemNum;

	return tabElem;

}


void echanger(elemT *tableau, int a, int b)
{
	elemT temp = tableau[a];
	tableau[a] = tableau[b];
	tableau[b] = temp;
}

void quickSort(elemT *tableau, int debut, int fin, colonne *c)
{
	int gauche = debut-1;
	int droite = fin+1;



	/* Si le tableau est de longueur nulle, il n'y a rien à faire. */
	if(debut >= fin)
		return;

	/* Sinon, on parcourt le tableau, une fois de droite à gauche, et une
	   autre de gauche à droite, à la recherche d'éléments mal placés,
	   que l'on permute. Si les deux parcours se croisent, on arrête. */

	switch(c->type)
	{
	case INT:
	{
		const int *pivot = tableau[debut].valeur;

		while(1)
		{

			int *valD = malloc(sizeof(int));
			int *valG = malloc(sizeof(int));

			do {
				droite--;
				valD = tableau[droite].valeur;
			} while(*valD > *pivot);

			do {
				gauche++;
				valG = tableau[gauche].valeur;
			}  while(*valG < *pivot);

			if(gauche < droite)
				echanger(tableau, gauche, droite);
			else
				break;

		}
		break;
	}
	case CHAR:
	{
		const char *pivot = tableau[debut].valeur;

		while(1)
		{
			char *valD = malloc(c->size+1);
			char *valG = malloc(c->size+1);

			do {
				droite--;
				valD = tableau[droite].valeur;
			} while(memcmp(valD, pivot, strlen(valD)+1) > 0);

			do {
				gauche++;
				valG = tableau[gauche].valeur;
			}  while(memcmp(valG, pivot, strlen(valD)+1) < 0);

			if(gauche < droite)
				echanger(tableau, gauche, droite);
			else
				break;
		}
		break;

	}
	case FLOAT:
	{
		const float *pivot = tableau[debut].valeur;

		while(1)
		{

			float *valD = malloc(sizeof(float));
			float *valG = malloc(sizeof(float));

			do {
				droite--;
				valD = tableau[droite].valeur;
			} while(*valD > *pivot);

			do {
				gauche++;
				valG = tableau[gauche].valeur;
			}  while(*valG < *pivot);

			if(gauche < droite)
				echanger(tableau, gauche, droite);
			else
				break;

		}
		break;
	}
	case DOUBLE:
	{
		const double *pivot = tableau[debut].valeur;

		while(1)
		{
			double *valD = malloc(sizeof(double));
			double *valG = malloc(sizeof(double));

			do {
				droite--;
				valD = tableau[droite].valeur;
			} while(*valD > *pivot);

			do {
				gauche++;
				valG = tableau[gauche].valeur;
			}  while(*valG < *pivot);

			if(gauche < droite)
				echanger(tableau, gauche, droite);
			else
				break;

		}
		break;
	}
	case DATE:
	{
		struct tm *pivot = tableau[debut].valeur;
		pivot->tm_hour = pivot->tm_min = pivot->tm_sec = 0;

		while(1)
		{
			struct tm *valD = malloc(sizeof(struct tm));
			struct tm *valG = malloc(sizeof(struct tm));

			do {
				droite--;
				valD = tableau[droite].valeur;
				valD->tm_hour = valD->tm_min = valD->tm_sec = 0;
			} while(difftime(mktime(valD), mktime(pivot)) > 0);

			do {
				gauche--;
				valG = tableau[gauche].valeur;
				valG->tm_hour = valG->tm_min = valG->tm_sec = 0;
			} while(difftime(mktime(valG), mktime(pivot)) < 0);

			if(gauche < droite)
				echanger(tableau, gauche, droite);
			else
				break;
		}
		break;
	}

	}



	/* Maintenant, tous les éléments inférieurs au pivot sont avant ceux
	   supérieurs au pivot. On a donc deux groupes de cases à trier. On utilise
	   pour cela... la méthode quickSort elle-même ! */
	quickSort(tableau, debut, droite, c);
	quickSort(tableau, droite+1, fin, c);

}



int comparerElements(TYPE t, size_t taille, extension *pE, extension *pE2, void *row1, void *row2, int decCol1, int decCol2)
{
	int verif = 1;

	switch(t) {
	case INT:
	{
		//Valeur 1
		int *val1 = malloc(sizeof(int));
		memcpy(val1, row1 + decCol1, sizeof(int));

		//Valeur 2
		int *val2 = malloc(sizeof(int));
		memcpy(val2, row2 + decCol2, sizeof(int));


		if(*val1 != *val2)
			verif = 0;

		free(val1);
		free(val2);

		break;
	}
	case CHAR:
	{
		//Valeur 1
		char *val1;
		val1 = malloc(taille);
		memcpy(val1, row1 + decCol1, taille);

		//Valeur 2
		char *val2;
		val2 = malloc(taille);
		memcpy(val2, row2 + decCol2, taille);

		if(memcmp(val1, val2, strlen(val2)+1) != 0)
			verif = 0;

		free(val1);
		free(val2);

		break;
	}
	case FLOAT:
	{
		//Valeur 1
		float *val1;
		val1 = malloc(sizeof(float));
		memcpy(val1, row1 + decCol1, sizeof(float));

		//Valeur 2
		float *val2;
		val2 = malloc(sizeof(float));
		memcpy(val2, row2 + decCol2, sizeof(float));

		if(*val1 != *val2)
			verif = 0;

		free(val1);
		free(val2);

		break;
	}
	case DOUBLE:
	{
		//Valeur 1
		double *val1;
		val1 = malloc(sizeof(double));
		memcpy(val1, row1 + decCol1, sizeof(double));

		//Valeur 2
		double *val2;
		val2 = malloc(sizeof(double));
		memcpy(val2, row2 + decCol2, sizeof(double));

		if(*val1 != *val2)
			verif = 0;

		free(val1);
		free(val2);

		break;
	}

	case DATE:
	{
		//Valeur 1
		struct tm *date1 = malloc(sizeof(struct tm));
		memcpy(date1, row1 + decCol1, sizeof(struct tm));

		struct tm *date2 = malloc(sizeof(struct tm));
		memcpy(date2, row2 + decCol2, sizeof(struct tm));

		date1->tm_hour = date1->tm_min = date1->tm_sec = 0;
		date2->tm_hour = date2->tm_min = date2->tm_sec = 0;

		double dif = difftime(mktime(date1), mktime(date2));

		if(dif != 0)
			verif = 0;

		free(date1);
		free(date2);

		break;
	}

	}

	return verif;

}




void printJoinResult(FNode *fn, query *q)
{

	while(fn->prev != NULL)
		fn = fn->prev;

	extension *pE = malloc(sizeof(extension));

	int *tabDecCol = malloc(sizeof(int) * q->nbColonnes);

	int totalSize;
	int compteur = 0;

	void *row;

	pE = &fn->e;

	//Verification et affichage du nom des colonnes
	for(int col=0; col<q->nbColonnes; col++)
	{
		totalSize = 0;

		//Phase de vérification de l'existence des colonnes
		int i;
		for(i=0; i<pE->fp.h.t.nbColonnes; i++)
		{
			if(memcmp(q->tabColonne[col].nom, pE->fp.h.t.listeColonnes[i].nom, strlen(pE->fp.h.t.listeColonnes[i].nom)+1) == 0)
			{
				q->tabColonne[col] = pE->fp.h.t.listeColonnes[i];
				break;
			}
			totalSize += pE->fp.h.t.listeColonnes[i].size;
		}

		if(memcmp(q->tabColonne[col].nom, pE->fp.h.t.listeColonnes[i].nom, strlen(pE->fp.h.t.listeColonnes[i].nom)+1) != 0)
		{
			printf("Unknown column : %s\n", q->tabColonne[col].nom);
		}

		//Phase d'affichage des colonnes
		printf("%s\t\t", q->tabColonne[col].nom);

		tabDecCol[col] = totalSize;
	}

	printf("\n");

	for(int i=0;i<q->nbColonnes;i++) {
		printf("----------------------- ");
	}

	printf("\n");



	while(fn != NULL)
	{
		pE = &fn->e;

		for(int bloc=0; bloc<7; bloc++)
		{
			for(int elem=0; elem<pE->p[bloc].h.nbElements; elem++)
			{
				row = pE->p[0].data + elem * pE->fp.h.t.tailleElem;

				for(int k=0;k<q->nbColonnes;k++)
				{
					afficherValeur(pE, q->tabColonne[k], row, tabDecCol[k]);
				}

				printf("\n");

				compteur++;
			}
		}

		fn = fn->next;
	}

	printf("\nNumber of join elements : %d\n", compteur);
}



//Créer la table jointe de deux table passées en paramètres
void newJoinTable(FNode *fn, extension *pE1, extension *pE2)
{
	extension *pE = malloc(sizeof(extension));

	//Copie des colonnes
	for(int i=0; i<pE1->fp.h.t.nbColonnes; i++)
	{
		pE->fp.h.t.listeColonnes[i] = pE1->fp.h.t.listeColonnes[i];
		if(memcmp(pE1->fp.h.t.nom, "joinTable", strlen("joinTable")+1) != 0)
		{
			char *nomCol = malloc(strlen(pE1->fp.h.t.nom) + strlen(pE1->fp.h.t.listeColonnes[i].nom)+2);
			memcpy(nomCol, pE1->fp.h.t.nom, strlen(pE1->fp.h.t.nom)+1);
			strcat(nomCol, ".");
			strcat(nomCol, pE1->fp.h.t.listeColonnes[i].nom);
			memcpy(pE->fp.h.t.listeColonnes[i].nom, nomCol, strlen(nomCol)+1);
		}
	}

	pE->fp.h.t.nbColonnes = pE1->fp.h.t.nbColonnes;

	for(int j=0; j<pE2->fp.h.t.nbColonnes; j++)
	{
		int i = j + pE->fp.h.t.nbColonnes;
		pE->fp.h.t.listeColonnes[i] = pE2->fp.h.t.listeColonnes[j];
		if(memcmp(pE2->fp.h.t.nom, "joinTable", strlen("joinTable")+1) != 0)
		{
			char *nomCol = malloc(strlen(pE2->fp.h.t.nom) + strlen(pE2->fp.h.t.listeColonnes[j].nom)+2);
			memcpy(nomCol, pE2->fp.h.t.nom, strlen(pE2->fp.h.t.nom)+1);
			strcat(nomCol, ".");
			strcat(nomCol, pE2->fp.h.t.listeColonnes[j].nom);
			memcpy(pE->fp.h.t.listeColonnes[i].nom, nomCol, strlen(nomCol)+1);
		}
	}

	pE->fp.h.t.nbColonnes += pE2->fp.h.t.nbColonnes;

	//Copie de la taille d'un élément
	pE->fp.h.t.tailleElem = pE1->fp.h.t.tailleElem + pE2->fp.h.t.tailleElem;
	pE->id = 0;

	memcpy(pE->fp.h.t.nom, "joinTable", strlen("joinTable")+1);

	for(int i=0; i<7; i++)
		pE->p[i].h.nbElements = 0;

	fn->e = *pE;
	fn->next = fn->prev = NULL;

}



//Insère un élément dans la table jointe
void insertIntoJoinTable(FNode *fn, extension *pE1, extension *pE2, void *row1, void *row2)
{
	int bloc = -1;
	int decEcri, totalSize;

	extension *pE = malloc(sizeof(extension));

	do
	{
		pE = &fn->e;
		for(int i=0; i<7; i++)
		{
			if(pE->p[i].h.nbElements * pE->fp.h.t.tailleElem + pE->fp.h.t.tailleElem < (int)sizeof(pE->p[i].data))
			{
				bloc = i;
				break;
			}
		}
		if(fn->next != NULL)
			fn = fn->next;
	} while(fn->next != NULL);

	if(bloc == -1)
	{
		FNode *fn2 = malloc(sizeof(FNode));
		fn2->e.fp.h.t = fn->e.fp.h.t;
		fn->next = fn2;
		fn2->prev = fn;
		fn = fn2;
		fn->e.id = fn->prev->e.id+1;
		for(int i=0; i<7; i++)
			fn->e.p[i].h.nbElements = 0;
		pE = &fn->e;
		bloc = 0;
	}

	int rowId = 0;
	for(int i=0; i<fn->e.p[bloc].h.nbElements; i++)
		rowId++;

	decEcri = rowId * pE->fp.h.t.tailleElem;
	totalSize = 0;

	for(int i=0;i<pE1->fp.h.t.nbColonnes;i++) {

		switch(pE->fp.h.t.listeColonnes[i].type) {
		case INT:
		{
			memcpy(pE->p[bloc].data + decEcri + totalSize, row1 + totalSize, sizeof(int));
			break;
		}
		case CHAR:
		{
			memcpy(pE->p[bloc].data + decEcri + totalSize, row1 + totalSize, pE1->fp.h.t.listeColonnes[i].size);
			break;
		}
		case FLOAT:
		{
			memcpy(pE->p[bloc].data + decEcri + totalSize, row1 + totalSize, sizeof(float));
			break;
		}

		case DOUBLE:
		{
			memcpy(pE->p[bloc].data + decEcri + totalSize, row1 + totalSize, sizeof(double));
			break;
		}

		case DATE:
		{
			memcpy(pE->p[bloc].data + decEcri + totalSize, row1 + totalSize, sizeof(struct tm));
			break;
		}
		}

		totalSize += pE->fp.h.t.listeColonnes[i].size;

	}

	int totalSize2 = 0;

	for(int i=0;i<pE2->fp.h.t.nbColonnes;i++) {

		switch(pE2->fp.h.t.listeColonnes[i].type) {
		case INT:
		{
			memcpy(pE->p[bloc].data + decEcri + totalSize, row2 + totalSize2, sizeof(int));
			break;
		}
		case CHAR:
		{
			memcpy(pE->p[bloc].data + decEcri + totalSize, row2 + totalSize2, pE2->fp.h.t.listeColonnes[i].size);
			break;
		}
		case FLOAT:
		{
			memcpy(pE->p[bloc].data + decEcri + totalSize, row2 + totalSize2, sizeof(float));
			break;
		}

		case DOUBLE:
		{
			memcpy(pE->p[bloc].data + decEcri + totalSize, row2 + totalSize2, sizeof(double));
			break;
		}

		case DATE:
		{
			memcpy(pE->p[bloc].data + decEcri + totalSize, row2 + totalSize2, sizeof(struct tm));
			break;
		}
		}

		totalSize2 += pE2->fp.h.t.listeColonnes[i].size;
		totalSize += pE2->fp.h.t.listeColonnes[i].size;

	}

	fn->e.p[bloc].h.nbElements++;
}


