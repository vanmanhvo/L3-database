/*
 * delete.c
 *
 *  Created on: 1 août 2016
 *      Author: Andy
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../Headers/tableOperations.h"
#include "../../Headers/parser.h"




void deleteFromTable(char *str)
{

	query *q = malloc(sizeof(query));

	initQuery(q, DELETE);

	//Récupère le nom de la Table
	q->nomTable = strtok(str, " ");
	if (q->nomTable == NULL) {
		printf("Syntax error : missing arguments\n");
		return;
	}



	char *w = strtok(NULL, " ");

	majuscule(w);

	if(memcmp(w, "WHERE", sizeof("WHERE")) != 0)
	{
		printf("Syntax error : missing key word WHERE\n");
		return;
	}


	char *where = strtok(NULL, "");


	if(where == NULL)
	{
		printf("Syntax error : no arguments after WHERE\n");
		return;
	}

	//Collecte de la clause WHERE
	char *token;

	char *restToken;

	int nbDelim = 0;

	//Declaration des variable
	char *colonne;
	char *operateur;
	char *valeur;

	char *copy;


	while(where != NULL)
	{

		token = where;

		//Copie de la string
		copy = malloc(strlen(token)+1);
		memcpy(copy, token, strlen(token)+1);

		char *debutCondi = copy;


		//Récupération de la colonne
		colonne = strtok(copy, "=><!");

		//Récupération de la valeur
		valeur = strtok(NULL, "=><!");

		if(valeur == NULL)
		{
			printf("Syntax error : missing value\n");
			return;
		}



		//Récupération de l'opérateur de comparaison

		operateur = calloc(sizeof(char), valeur-(colonne+strlen(colonne))+1);

		memcpy(operateur, token+strlen(colonne), valeur-(colonne+strlen(colonne)));

		operateur = strtok(operateur, " ");


		if(operateur[strlen(operateur)-1] == valeur[0])
			operateur[strlen(operateur)-1] = 0;


		if (memcmp(operateur, "=", strlen("=")+1) == 0) {}
		else if (memcmp(operateur, "!=", strlen("!=")+1) == 0) {}
		else if (memcmp(operateur, ">", strlen(">")+1) == 0) {}
		else if (memcmp(operateur, "<", strlen("<")+1) == 0) {}
		else if (memcmp(operateur, ">=", strlen(">=")+1) == 0) {}
		else if (memcmp(operateur, "<=", strlen("<=")+1) == 0) {}
		else
		{
			printf("Syntax error : unknown operator\n");
			printf("-%s-\n", operateur);
			return;
		}



		//Ajout des éléments dans leurs tableaux respectifs
		q->where.tabNomColonne[q->where.nbConditions] = strtok(colonne, " ");

		q->where.tabOperateur[q->where.nbConditions] = operateur;


		while(valeur[0] == ' ')
			valeur++;

		char *finCondi;


		//Valeur numérique
		if(valeur[0] != '\'')
		{
			//Ajout de la valeur au tableau des valeurs
			valeur = strtok(valeur, " ");

			if(valeur == NULL)
			{
				printf("Syntax error : no value\n");
				return;
			}

		}

		//Valeur non numérique
		else
			valeur = strtok(valeur, "'");


		finCondi = valeur + strlen(valeur)+1;

		q->where.tabValeur[q->where.nbConditions] = valeur;


		restToken = token + (finCondi - debutCondi);

		//On retire les espaces en début de chaine
		while(restToken[0] == ' ')
			restToken++;


		q->where.nbConditions++;

		int nextTokIsDelim = 0;

		if(restToken != NULL)
		{
			char *majCopy = malloc(strlen(restToken)+1);
			memcpy(majCopy, restToken, strlen(restToken)+1);
			majuscule(majCopy);

			char *nextTok = strtok(majCopy, " ");

			if(nextTok == NULL)
			{
				where = str = NULL;
				break;
			}


			if(memcmp(nextTok, "OR", strlen("OR")) == 0)
				nextTokIsDelim = 1;

			else if (memcmp(nextTok, "AND", strlen("AND")) == 0)
				nextTokIsDelim = 1;

			else
				nextTokIsDelim = 0;

			if(nextTokIsDelim != 0)
			{
				q->where.tabDelim[nbDelim] = nextTok;
				nbDelim++;
			}


			free(majCopy);

			strtok(restToken, " ");
			str = strtok(NULL, "");
			where = str;

		}

		if(nextTokIsDelim == 0)
			break;
	}


	printf("OK.\n");

	//Creer la fonction
	supprimerElementTableWhere(q);



}
