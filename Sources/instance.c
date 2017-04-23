/*
 * instance.c
 *
 *  Created on: 12 août 2016
 *      Author: Andy
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../Headers/timestamp.h"
#include "../Headers/dbwr.h"
#include "../Headers/parser.h"
#include "../Headers/instance.h"
#include "../Headers/outils.h"
#include "../Headers/tableOperations.h"
#include "../Headers/history.h"


#define TAILLE_MAX 1000



void readQuery(pthread_t t)
{
	char *query;
	char str[400];

	char *c = malloc(sizeof(char));
	//char requete[TAILLE_MAX] = "";
	while(1)
	{
		*c = '\0';
		query = calloc(sizeof(char), 400);
		printf("bql");
		while(*c != ';')
		{
				printf(">");
				fgets(str, sizeof(str), stdin);
				query[strlen(query)] = ' ';
				query[strlen(query)+1] = '\0';
				strcat(query, str);
				*c = query[strlen(query)-2];
		}


		while(query[0] == ' ')
			query++;



		if(memcmp(query, "exit", strlen("exit")) == 0)
		{
			scan();
			printf("Stopping database\n");
			pthread_cancel(t);
			break;
		}

		query[strlen(query)-2] = '\0';

		replace(query, '\n', ' ');

		sql(query);
		//free(query);
	}

}


void startDb()
{
	time_t timestamp;
	struct tm *info;

	time(&timestamp);

	info = localtime(&timestamp);

	logTime(asctime(info));	//logger l'heure courrante

	printf("Starting Big Data Application...\n");
	printf("Session launched : %s\n", asctime(info));
	//DBWR Thread
	pthread_t threadDbwr;
	pthread_t threadLecture;

	//Création du thread
	pthread_create(&threadDbwr, NULL, (void *)*startDbwr, NULL);
	pthread_create(&threadLecture, NULL, (void *)*readQuery, threadDbwr);

	pthread_join(threadDbwr, NULL);
	pthread_join(threadLecture, NULL);
}
