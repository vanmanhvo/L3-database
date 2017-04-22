/*
 * select.c
 *
 *  Created on: 2 août 2016
 *      Author: Andy
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../Headers/tableOperations.h"
#include "../../Headers/parser.h"


void selectFrom(char *str)
{
	query *q = malloc(sizeof(query));

	initQuery(q, SELECT);

	char *f;
	char *fCopy;

	char *colonnesSelect;

	char *token;


	//Copie de la string en majuscule pour trouver le mot clé FROM
	char *majCopy = malloc(strlen(str)+1);
	memcpy(majCopy, str, strlen(str)+1);
	majuscule(majCopy);

	fCopy = strstr(majCopy, "FROM");

	//On vérifie que le mot clé FROM est bien présent
	if(fCopy == NULL)
	{
		printf("Syntax error : no key word FROM\n");
		return;
	}

	free(majCopy);

	f = fCopy - majCopy + str - 1;

	//On isole la string contenant les colonnes à selectionner
	colonnesSelect = malloc(f-str+1);
	memcpy(colonnesSelect, str, f-str);
	colonnesSelect[f-str] = '\0';



	//Collecte des colonnes
	token = strtok(colonnesSelect, ", ");

	if(token == NULL)
	{
		printf("Syntax error : no column selected\n");
		return;
	}

	q->nbColonnes = 0;

	while(token != NULL)
	{
		memcpy(q->tabColonne[q->nbColonnes].nom, token, strlen(token));
		q->tabColonne[q->nbColonnes].nom[strlen(token)] = '\0';

		token = strtok(NULL, ", ");
		q->nbColonnes++;
	}


	//On réduit la string de départ aux éléments présents après le mot clé FROM
	str = f + strlen(" FROM ");

	//On récupère le nom de la table
	q->nomTable = strtok(str, " ");

	//On vérifie la présence du nom de la table
	if(q->nomTable == NULL)
	{
		printf("Syntax error : missing table name\n");
		return;
	}


	str = strtok(NULL, "");



	//Tant qu'il y a des commandes supplémentaires
	while(str != NULL)
	{

		char *commande;
		commande = strtok(str, " ");
		majuscule(commande);

		//CAS OU LA COMMANDE JOIN EST PRESENTE
		if(strncmp(commande, "JOIN", strlen("JOIN")+1) == 0)
		{

			char *join = strtok(NULL, "");

			if(join == NULL)
			{
				printf("Syntax error : missing arguments\n");
				return;
			}

			q->join[q->nbJoin].nomTable = malloc(sizeof(char) * 21);
			q->join[q->nbJoin].nomTable = strtok(join, " ");

			q->join[q->nbJoin].on.nbConditions = 0;

			//Vérification de la présence du mot clé ON
			char *on = strtok(NULL, " ");
			majuscule(on);

			if(strncmp(on, "ON", strlen("ON")+1) != 0)
			{
				printf("Syntax error : missing key word ON\n");
				return;
			}

			char *conditions = strtok(NULL, "");

			//Suppression des espaces devant la parenthèse des conditions
			while(conditions[0] == ' ')
				conditions++;

			if(conditions[0] != '(')
			{
				printf("Syntax error : missing '('\n");
				return;
			}

			conditions = strtok(conditions, "()");
			str = strtok(NULL, "");


			//Collecte des conditions
			char *delim[] = {" OR ", " AND "};

			int nbDelim = 0;

			char *pos;
			pos = malloc(strlen(conditions));

			char *x;


			while(conditions != NULL)
			{
				pos = NULL;

				//On cherche la première occurence de "OR" ou "AND"
				char *majCopy = malloc(strlen(conditions)+1);
				memcpy(majCopy, conditions, strlen(conditions)+1);
				majuscule(majCopy);

				for(int i=0;i<2;i++)
				{


					fCopy = strstr(majCopy, delim[i]);

					if(fCopy != NULL)
					{
						x = fCopy - majCopy + conditions;

						if(x < pos || pos == NULL)
						{
							pos = x;
						}
					}
				}

				free(majCopy);

				//Pas de OR ou AND
				if(pos == NULL)
				{
					token = malloc(sizeof(conditions));
					token = conditions;
					conditions = NULL;

				}



				//Présence de OR ou AND
				else
				{
					token = malloc(pos-conditions);

					memcpy(token, conditions, pos-conditions);
					token[pos-conditions] = '\0';

					q->join[q->nbJoin].on.tabDelim[nbDelim] = strtok(pos, " ");
					nbDelim++;

					conditions = strtok(NULL, "");
				}

				q->join[q->nbJoin].on.tabNomColonne[q->join[q->nbJoin].on.nbConditions] =  strtok(token, "= ");
				q->join[q->nbJoin].on.tabValeur[q->join[q->nbJoin].on.nbConditions] = strtok(NULL, "= ");

				q->join[q->nbJoin].on.nbConditions++;

			}

			q->nbJoin++;

		}

		//CAS OU LA COMMANDE WHERE EST PRESENTE
		else if(strncmp(commande, "WHERE", strlen("WHERE")+1) == 0)
		{
			char *where;
			where = strtok(NULL, "");

			//Collecte de la clause WHERE
			char *token;

			int nbDelim = 0;

			char *pos;
			pos = malloc(strlen(where));

			char *restToken;

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

		}

		else
		{
			printf("Syntax error : unknown command\n");
			return;
		}

	}


	printf("OK.\n");

	selectionner(q);

	free(colonnesSelect);
}

