/*
 * update.c
 *
 *  Created on: 27 juil. 2016
 *      Author: Andy
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../Headers/tableOperations.h"
#include "../../Headers/parser.h"


void updateTable(char *str)
{

	query *q = malloc(sizeof(query));

	initQuery(q, UPDATE);

	//Récupère le nom de la Table
	q->nomTable = strtok(str, " ");
	if (q->nomTable == NULL) {
		printf("Syntax error : missing arguments\n");
		return;
	}



	char *args = strtok(NULL, "");



	//Vérification de la présence du mot SET
	char *token = strtok(args, " ");
	majuscule(token);

	if(!memcmp(token, "SET", sizeof("SET")) == 0 || token == NULL) {
		printf("Syntax error : missing key word SET");
		return;
	}

	args = strtok(NULL, "");


	char *majCopy = malloc(strlen(args)+1);
	memcpy(majCopy, args, strlen(args)+1);
	majuscule(majCopy);

	char *fCopy = strstr(majCopy, "WHERE");
	char *w = NULL;


	//On regarde si le mot clé WHERE est présent
	if(fCopy != NULL)
	{
		w = fCopy - majCopy + args;
	}

	free(majCopy);




	char *subStr;
	char *restSubStr;


	//ABSENCE DE CLAUSE WHERE
	if(!w)
	{

		//Collecte des valeurs
		token = strtok(args, ",");
		char *restToken = strtok(NULL, "");

		if(token == NULL) {
			printf("Syntax error : no values\n");
			return;
		}

		while(token != NULL)
		{

			subStr = strtok(token, "=");
			restSubStr = strtok(NULL, "");

			if(restSubStr == NULL) {
				printf("Syntax error : no values\n");
				return;
			}

			char *tmp = strtok(subStr, " ");
			memcpy(q->tabColonne[q->nbValeurs].nom, tmp, strlen(tmp)+1);


			//On compte le nombre d'apostrophes
			int i, compteur;
			for (i=0, compteur=0; restSubStr[i]; i++) {
				compteur += (restSubStr[i] == '\'');
			}

			//Pas d'apostrophe, valeur numérique
			if(compteur == 0)
			{
				//Ajout de la valeur au tableau des valeurs
				q->tabValeurs[q->nbValeurs] = strtok(restSubStr, " ");

				if(strtok(NULL, "") != NULL)
				{
					printf("Syntax error : invalid value\n");
					return;
				}
			}



			//2 apostrophes, string
			else if (compteur == 2)
			{

				//On retire les espaces avant la string si il y en a
				while(restSubStr[0] != '\'') {

					if(restSubStr[0] != ' ') {
						printf("Syntax error : invalid value\n");
						return;
					}

					restSubStr++;
				}

				//Ajout de la string au tableau de valeurs
				q->tabValeurs[q->nbValeurs] = strtok(restSubStr, "'");

				if(strtok(NULL, "' ") != NULL)
				{
					printf("Syntax error : invalid value\n");
					return;
				}
			}

			else
			{
				printf("Syntax error : invalid value\n");
				return;
			}



			//Valeur suivante
			q->nbValeurs++;
			token = strtok(restToken, ",");
			restToken = strtok(NULL, "");
		}

		majTable(q);

	}

	//PRESENCE DE CLAUSE WHERE
	else {

		char *values;
		values = malloc(w-args+1);
		memcpy(values, args, strlen(args)-strlen(w));

		values[w-args] = 0;

		char *where = args + strlen(values);


		char *majCopy = malloc(strlen(where)+1);
		memcpy(majCopy, where, strlen(where)+1);
		majuscule(majCopy);

		if(memcmp(strtok(majCopy, " "), "WHERE", sizeof("WHERE")) != 0)
		{
			printf("Syntax error : where clause\n");
			return;
		}

		free(majCopy);

		strtok(where, " ");
		where = strtok(NULL, "");


		//Collecte des valeurs
		token = strtok(values, ",");
		char *restToken = strtok(NULL, "");


		if(token == NULL) {
			printf("Syntax error : no values\n");
			return;
		}

		while(token != NULL)
		{

			subStr = strtok(token, "=");
			restSubStr = strtok(NULL, "");

			if(restSubStr == NULL) {
				printf("Syntax error : no values\n");
				return;
			}

			char *tmp = strtok(subStr, " ");
			memcpy(q->tabColonne[q->nbValeurs].nom, tmp, strlen(tmp)+1);



			//On compte le nombre d'apostrophes
			int i, compteur;
			for (i=0, compteur=0; restSubStr[i]; i++) {
				compteur += (restSubStr[i] == '\'');
			}

			//Pas d'apostrophe, valeur numérique
			if(compteur == 0)
			{
				//Ajout de la valeur au tableau des valeurs
				q->tabValeurs[q->nbValeurs] = strtok(restSubStr, " ");

				if(strtok(NULL, "") != NULL)
				{
					printf("Syntax error : invalid value\n");
					return;
				}
			}



			//2 apostrophes, string
			else if (compteur == 2)
			{

				//On retire les espaces avant la string si il y en a
				while(restSubStr[0] != '\'') {

					if(restSubStr[0] != ' ') {
						printf("Syntax error : invalid value\n");
						return;
					}

					restSubStr++;
				}

				//Ajout de la string au tableau de valeurs
				q->tabValeurs[q->nbValeurs] = strtok(restSubStr, "'");

				if(strtok(NULL, "' ") != NULL)
				{
					printf("Syntax error : invalid value\n");
					return;
				}
			}

			else
			{
				printf("Syntax error : invalid value\n");
				return;
			}



			//Valeur suivante
			q->nbValeurs++;
			token = strtok(restToken, ",");
			restToken = strtok(NULL, "");
		}




		//Collecte de la clause WHERE

		if(where == NULL)
		{
			printf("Syntax error : no arguments after WHERE\n");
			return;
		}


		char *token;

		int nbDelim = 0;

		char *pos;
		pos = malloc(strlen(where));


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

			memcpy(operateur, token+strlen(colonne), valeur-(colonne+strlen(colonne))+1);

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

		free(copy);


		printf("OK.\n");

		//Creer la fonction
		majTable(q);
	}

}
