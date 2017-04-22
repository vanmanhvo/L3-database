/*
 * create.c
 *
 *  Created on: 18 juil. 2016
 *      Author: Andy
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../../Headers/outils.h"
#include "../../Headers/tableOperations.h"
#include "../../Headers/parser.h"



void createTable(char *str)
{
	query *q = malloc(sizeof(query));

	initQuery(q, CREATE_TABLE);


	//Récupère le nom de la Table
	q->nomTable = strtok(str, " ");
	if (q->nomTable == NULL) {
		printf("Syntax error : missing arguments\n");
		return;
	}

	char *args = strtok(NULL, "");

	//Verification et suppression des parenthèses des arguments
	if(( *args != '(' ) || ( args[strlen(args)-1] != ')' ))
	{
		printf("Syntax error : missing ( )");
		return;
	}


	else {
		args++;
		args[strlen(args)-1] = 0;
	}


	//Collecte des colonnes

	char *token;
	char *restToken;

	//Isole la première colonne
	token = strtok(args, ",");
	restToken = strtok(NULL, "");


	//Isole chaque string de chaque colonne
	char *subToken;


	while(token != NULL)
	{
		//Compte le nombre de mots
		int nbMots = 0;

		const char *it = token;
		int inword = 0;

		do switch(*it) {
		case '\0':
		case ' ': case '\t': case '\n': case '\r':
			if (inword) { inword = 0; nbMots++; }
			break;
		default: inword = 1;
		} while(*it++);


		if(nbMots<2) {
			printf("Syntax error : missing arguments\n");
			return;
		}


		q->tabColonne[q->nbColonnes].nbConstraint = 0;

		subToken = strtok(token, " ");

		//Nom de la colonne
		memcpy(q->tabColonne[q->nbColonnes].nom, subToken, sizeof(q->tabColonne[q->nbColonnes].nom));

		subToken = strtok(NULL, " (");

		if(subToken == NULL) {
			printf("Syntax argument : missing type\n");
			return;
		}


		else {

			majuscule(subToken);

			//Type de la colonne
			if(memcmp(subToken, "INT", sizeof("INT")) == 0)
			{
				q->tabColonne[q->nbColonnes].type = INT;
				q->tabColonne[q->nbColonnes].nb = 1;
				q->tabColonne[q->nbColonnes].size = sizeof(int);
			}

			else if(memcmp(subToken, "VARCHAR", sizeof("VARCHAR")) == 0)
			{
				q->tabColonne[q->nbColonnes].type = CHAR;
				subToken = strtok(NULL, ")");
				char *endPtr;
				endPtr = NULL;
				int taille = strtol(subToken, &endPtr, 10);

				//On vérifie que le nombre est bien valide
				if(taille == 0 || (endPtr != subToken && *endPtr != '\0')) {
					printf("Syntax error : invalid number\n");
					return;
				}

				q->tabColonne[q->nbColonnes].nb = taille;
				//+1 pour le caractère de fin \0
				q->tabColonne[q->nbColonnes].size = taille + 1 * sizeof(char);

			}


			else if(memcmp(subToken, "NUMERIC", sizeof("NUMERIC")) == 0)
			{
				q->tabColonne[q->nbColonnes].type = FLOAT;
				q->tabColonne[q->nbColonnes].nb = 1;
				q->tabColonne[q->nbColonnes].size = sizeof(float);
			}

			else if(memcmp(subToken, "DOUBLE", sizeof("DOUBLE")) == 0)
			{
				q->tabColonne[q->nbColonnes].type = DOUBLE;
				q->tabColonne[q->nbColonnes].nb = 1;
				q->tabColonne[q->nbColonnes].size = sizeof(double);
			}

			else if(memcmp(subToken, "DATE", sizeof("DATE")) == 0)
			{
				q->tabColonne[q->nbColonnes].type = DATE;
				q->tabColonne[q->nbColonnes].nb = 1;
				q->tabColonne[q->nbColonnes].size = sizeof(struct tm);
			}

			/*else if(memcmp(subToken, "BIGINT", sizeof("BIGINT")) == 0)
			{
				q->tabColonne[q->nbColonnes].type = DATE;
				q->tabColonne[q->nbColonnes].nb = 1;
				q->tabColonne[q->nbColonnes].size = sizeof(struct tm);
			}*/

			else
			{
				printf("Syntax error : unknown type\n");
				return;
			}
		}


		//Contraintes de colonnes

		for(int compteurMot = 2; compteurMot<nbMots;compteurMot++) {

			subToken = strtok(NULL, " ");

			majuscule(subToken);

			//PRIMARY KEY
			if(memcmp(subToken, "PRIMARY", sizeof("PRIMARY")) == 0)
			{

				if((nbMots-compteurMot) < 1) {
					printf("Syntax error : unknown constraint\n");
					return;
				}

				compteurMot++;

				subToken = strtok(NULL, " ");


				if(memcmp(subToken, "KEY", sizeof("KEY")) == 0)
				{
					q->tabColonne[q->nbColonnes].constraint[q->tabColonne[q->nbColonnes].nbConstraint] = PRIMARY_KEY;
					q->tabColonne[q->nbColonnes].nbConstraint++;
				}

				else {
					printf("Syntax error : unknown constraint\n");
					return;
				}

			}




			else if(memcmp(subToken, "DEFAULT", sizeof("DEFAULT")) == 0)
			{

				if((nbMots-compteurMot) < 1) {
					printf("Syntax error : unknown constraint\n");
					return;
				}

				compteurMot++;

				subToken = strtok(NULL, " ");


				//Cast du type de la valeur par défaut
				switch(q->tabColonne[q->nbColonnes].type) {

				case INT:
				{
					char *endPtr;
					endPtr = NULL;
					int *value = NULL;
					*value = strtol(subToken, &endPtr, 10);

					//On vérifie que le nombre est bien valide
					if(endPtr != subToken && *endPtr != '\0') {
						printf("Syntax error : invalid default value\n");
						return;
					}

					q->tabColonne[q->nbColonnes].defaultValue = value;

					break;
				}

				case CHAR:
					q->tabColonne[q->nbColonnes].defaultValue = subToken;

					break;

				case FLOAT:
				{
					char *endPtr;
					endPtr = NULL;
					float *value = NULL;
					*value = strtof(subToken, &endPtr);

					//On vérifie que le nombre est bien valide
					if(endPtr != subToken && *endPtr != '\0')
					{
						printf("Syntax error : invalid default value\n");
						return;
					}


					q->tabColonne[q->nbColonnes].defaultValue = value;

					break;
				}

				case DOUBLE:
				{

					char *endPtr;
					endPtr = NULL;
					double *value = NULL;
					*value = strtod(subToken, &endPtr);

					//On vérifie que le nombre est bien valide
					if(endPtr != subToken && *endPtr != '\0')
					{
						printf("Syntax error : invalid default value\n");
						return;
					}


					q->tabColonne[q->nbColonnes].defaultValue = value;

					break;


				}

				case DATE:
				{
					char *endPtr;
					endPtr = NULL;
					double *value = NULL;
					*value = strtod(subToken, &endPtr);

					//On vérifie que le nombre est bien valide
					if(endPtr != subToken && *endPtr != '\0')
					{
						printf("Syntax error : invalid default value\n");
						return;
					}


					q->tabColonne[q->nbColonnes].defaultValue = value;

					break;
				}

				case BLOB:
				{
					char *endPtr;
					endPtr = NULL;
					double *value = NULL;
					*value = strtod(subToken, &endPtr);

					//On vérifie que le nombre est bien valide
					if(endPtr != subToken && *endPtr != '\0')
					{
						printf("Syntax error : invalid default value\n");
						return;
					}


					q->tabColonne[q->nbColonnes].defaultValue = value;

					break;
				}

				case ACSII:
				{
					char *endPtr;
					endPtr = NULL;
					double *value = NULL;
					*value = strtod(subToken, &endPtr);

					//On vérifie que le nombre est bien valide
					if(endPtr != subToken && *endPtr != '\0')
					{
						printf("Syntax error : invalid default value\n");
						return;
					}


					q->tabColonne[q->nbColonnes].defaultValue = value;

					break;
				}

				case TEXT:
				{
					char *endPtr;
					endPtr = NULL;
					double *value = NULL;
					*value = strtod(subToken, &endPtr);

					//On vérifie que le nombre est bien valide
					if(endPtr != subToken && *endPtr != '\0')
					{
						printf("Syntax error : invalid default value\n");
						return;
					}


					q->tabColonne[q->nbColonnes].defaultValue = value;

					break;
				}

				case VARINT:
				{
					char *endPtr;
					endPtr = NULL;
					double *value = NULL;
					*value = strtod(subToken, &endPtr);

					//On vérifie que le nombre est bien valide
					if(endPtr != subToken && *endPtr != '\0')
					{
						printf("Syntax error : invalid default value\n");
						return;
					}


					q->tabColonne[q->nbColonnes].defaultValue = value;

					break;
				}

				case UUID:
				{
					char *endPtr;
					endPtr = NULL;
					double *value = NULL;
					*value = strtod(subToken, &endPtr);

					//On vérifie que le nombre est bien valide
					if(endPtr != subToken && *endPtr != '\0')
					{
						printf("Syntax error : invalid default value\n");
						return;
					}


					q->tabColonne[q->nbColonnes].defaultValue = value;

					break;
				}

				case TIMEUUID:
				{
					char *endPtr;
					endPtr = NULL;
					double *value = NULL;
					*value = strtod(subToken, &endPtr);

					//On vérifie que le nombre est bien valide
					if(endPtr != subToken && *endPtr != '\0')
					{
						printf("Syntax error : invalid default value\n");
						return;
					}


					q->tabColonne[q->nbColonnes].defaultValue = value;

					break;
				}

				case BOOLEAN:
				{
					char *endPtr;
					endPtr = NULL;
					double *value = NULL;
					*value = strtod(subToken, &endPtr);

					//On vérifie que le nombre est bien valide
					if(endPtr != subToken && *endPtr != '\0')
					{
						printf("Syntax error : invalid default value\n");
						return;
					}


					q->tabColonne[q->nbColonnes].defaultValue = value;

					break;
				}

				case DECIMAL:
				{
					char *endPtr;
					endPtr = NULL;
					double *value = NULL;
					*value = strtod(subToken, &endPtr);

					//On vérifie que le nombre est bien valide
					if(endPtr != subToken && *endPtr != '\0')
					{
						printf("Syntax error : invalid default value\n");
						return;
					}


					q->tabColonne[q->nbColonnes].defaultValue = value;

					break;
				}

				case COUNTER:
				{
					char *endPtr;
					endPtr = NULL;
					double *value = NULL;
					*value = strtod(subToken, &endPtr);

					//On vérifie que le nombre est bien valide
					if(endPtr != subToken && *endPtr != '\0')
					{
						printf("Syntax error : invalid default value\n");
						return;
					}


					q->tabColonne[q->nbColonnes].defaultValue = value;

					break;
				}


				case BIGINT:
				{
					break;
				}
				q->tabColonne[q->nbColonnes].constraint[q->tabColonne[q->nbColonnes].nbConstraint] = DEFAULT;
				q->tabColonne[q->nbColonnes].nbConstraint++;

				}
			}


			else if(memcmp(subToken, "NOT", sizeof("NOT")) == 0)
			{

				if((nbMots-compteurMot) < 1) {
					printf("Syntax error : unknown constraint\n");
					return;
				}

				compteurMot++;

				subToken = strtok(NULL, " ");
				majuscule(subToken);

				if(memcmp(subToken, "NULL", sizeof("NULL")) == 0)
				{
					q->tabColonne[q->nbColonnes].constraint[q->tabColonne[q->nbColonnes].nbConstraint] = NOT_NULL;
					q->tabColonne[q->nbColonnes].nbConstraint++;
					//printf(" NOT NULL");
				}

				else {
					printf("Syntax error : unknown constraint\n");
					return;
				}

			}


			//FOREIGN KEY
			else if(memcmp(subToken, "FOREIGN", sizeof("FOREIGN")) == 0)
			{

				if((nbMots-compteurMot) < 1) {
					printf("Syntax error : unknown constraint\n");
					return;
				}

				compteurMot++;

				subToken = strtok(NULL, " ");
				majuscule(subToken);

				if(memcmp(subToken, "KEY", sizeof("KEY")) == 0)
				{
					q->tabColonne[q->nbColonnes].constraint[q->tabColonne[q->nbColonnes].nbConstraint] = FOREIGN_KEY;
					q->tabColonne[q->nbColonnes].nbConstraint++;
				}

				else {
					printf("Syntax error : unknown constraint\n");
					return;
				}

			}


			//UNIQUE
			else if(memcmp(subToken, "UNIQUE", sizeof("UNIQUE")) == 0)
			{
				q->tabColonne[q->nbColonnes].constraint[q->tabColonne[q->nbColonnes].nbConstraint] = UNIQUE;
				q->tabColonne[q->nbColonnes].nbConstraint++;
			}


			//CHECK
			else if(memcmp(subToken, "CHECK", sizeof("CHECK")) == 0)
			{
				q->tabColonne[q->nbColonnes].constraint[q->tabColonne[q->nbColonnes].nbConstraint] = CHECK;
				q->tabColonne[q->nbColonnes].nbConstraint++;
			}


			//INDEX
			else if(memcmp(subToken, "INDEX", sizeof("INDEX")) == 0)
			{
				q->tabColonne[q->nbColonnes].constraint[q->tabColonne[q->nbColonnes].nbConstraint] = INDEX;
				q->tabColonne[q->nbColonnes].nbConstraint++;
				//printf(" INDEX");
			}


			else
			{
				printf("Syntax error : unknown constraint\n");
				return;
			}


			compteurMot++;

		}


		//Token suivant (colonne suivante)
		token = strtok(restToken, ",");
		restToken = strtok(NULL, "");

		q->nbColonnes++;
	}


	printf("OK.\n");

	//Création de la table avec les paramètres de la commande
	creerTable(q);


}
