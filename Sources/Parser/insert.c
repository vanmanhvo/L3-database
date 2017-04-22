/*
 * insert.c
 *
 *  Created on: 20 juil. 2016
 *      Author: Andy
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../Headers/tableOperations.h"
#include "../../Headers/parser.h"


void insertIntoTable(char *str)
{

	query *q = malloc(sizeof(query));

	initQuery(q, INSERT);

	//Récupère le nom de la Table
	q->nomTable = strtok(str, " ");
	if (q->nomTable == NULL) {
		printf("Syntax error : missing arguments\n");
		return;
	}


	char *args = strtok(NULL, "");


	//Vérification de la présence du mot VALUES
	char *token = strtok(args, " ");
	majuscule(token);

	if(!memcmp(token, "VALUES", sizeof("VALUES")) == 0 || token == NULL) {
		printf("Syntax error : missing key word VALUES");
		return;
	}


	//Récupération du reste de la chaine de charactères
	args = strtok(NULL, "");


	/*
	 * Il y aura une étape supplémentaire à rajouter pour le cas où les colonnes
	 * des arguments sont passées aussi en paramètres.
	 * Du coup il y aurait deux autres parenthèses à supprimer et d'autres vérifications à effectuer
	 */

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



	//Collecte des valeurs
	token = strtok(args, ",");
	char *restToken = strtok(NULL, "");


	if(token == NULL) {
		printf("Syntax error : no values\n");
		return;
	}

	while(token != NULL)
	{
		//char *copy = strdup(args);

		//On compte le nombre d'apostrophes
		int i, compteur;
		for (i=0, compteur=0; token[i]; i++) {
			compteur += (token[i] == '\'');
		}


		//Pas d'apostrophe, valeur numérique OU date
		if(compteur == 0)
		{

			//Ajout de la valeur au tableau des valeurs
			q->tabValeurs[q->nbValeurs] = strtok(token, " ");

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
			while(token[0] != '\'') {

				if(token[0] != ' ') {
					printf("Syntax error : invalid value\n");
					return;
				}

				token++;
			}

			//Ajout de la string au tableau de valeurs
			q->tabValeurs[q->nbValeurs] = strtok(token, "'");

			if(strtok(NULL, "' ") != NULL)
			{
				printf("Syntax error : invalid value\n");
				printf("%s\n", token);
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


	printf("OK.\n");

	insererValeurs(q);


}
