/*
 * parser.c
 *
 *  Created on: 17 juil. 2016
 *      Author: Andy
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../../Headers/parser.h"
#include "../../Headers/outils.h"
#include "../../Headers/tableOperations.h"

void sql(char *command) {

	logQuery(command);
	char *queryType = malloc(200);

	queryType = strtok(command, " ");
	majuscule(queryType);

	//CREATE FUNCTIONS
	if(memcmp(queryType, "CREATE", sizeof("CREATE")) == 0)
	{

		queryType = ("%s\n", strtok(NULL, " "));
		majuscule(queryType);

		//CREATE TABLE
		if(memcmp(queryType, "TABLE", sizeof("TABLE")) == 0)
		{
			createTable(strtok(NULL, ""));
		}

		else
		{
			printf("Commande inexistante\n");
		}
	}

	//INSERT FUNCTIONS
	else if(memcmp(queryType, "INSERT", sizeof("INSERT")) == 0)
	{

		queryType = ("%s\n", strtok(NULL, " "));
		majuscule(queryType);

		//INSERT INTO
		if(memcmp(queryType, "INTO", sizeof("INTO")) == 0)
		{

			queryType = ("%s\n", strtok(NULL, " "));
			majuscule(queryType);


			//INSERT INTO TABLE
			if(memcmp(queryType, "TABLE", sizeof("TABLE")) == 0) {

				insertIntoTable(strtok(NULL, ""));

			}

			else
			{
				printf("Commande inexistante\n");
			}

		}

		else
		{
			printf("Commande inexistante\n");
		}

	}

	//UPDATE FUNCTIONS
	else if(memcmp(queryType, "UPDATE", sizeof("UPDATE")) == 0)
	{

		updateTable(strtok(NULL, ""));

	}

	//DELETE FUNCTIONS
	else if(memcmp(queryType, "DELETE", sizeof("DELETE")) == 0)
	{

		queryType = ("%s\n", strtok(NULL, " "));
		majuscule(queryType);

		//DELETE FROM
		if(memcmp(queryType, "FROM", sizeof("FROM")) == 0)
		{

			deleteFromTable(strtok(NULL, ""));

		}

	}


	//UPDATE FUNCTIONS
	else if(memcmp(queryType, "SELECT", sizeof("SELECT")) == 0)
	{

		selectFrom(strtok(NULL, ""));

	}


	else
	{
		printf("Commande inexistante\n");
	}

}



void initQuery(query *q, queryType type)
{
	q->type = type;
	q->nomTable = malloc(sizeof(char) * 21);
	q->nbColonnes = 0;
	q->nbValeurs = 0;
	q->where.nbConditions = 0;
	q->nbJoin = 0;

}

char* majuscule(char *chaine)
{
    int i = 0;
    for (i = 0 ; chaine[i] != '\0' ; i++)
    {
        chaine[i] = toupper(chaine[i]);
    }
    return chaine;
}


