/*
 * testUnit.c
 *
 *  Created on: 22 avr. 2017
 *      Author: vovanmanh
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>


#include "../Headers/tableOperations.h"
#include "../Headers/LRUCache.h"
#include "../Headers/parser.h"
#include "../Headers/dbwr.h"
#include "../Headers/instance.h"
#include "../Headers/outils.h"
#include "../Headers/history.h"
#include "../Headers/editor.h"
#include "../Headers/typedef.h"

/****************************************TESTS UNITAIRES**************************************************/

void hist_TEST()
{
	addHistoryEntry("00");
	addHistoryEntry("01");
	addHistoryEntry("02");

	/*printf("%s\n", getCurrentEntry()); //03
	moveToPreviousHistEntry();


	printf("%s\n", getCurrentEntry()); //02
	moveToPreviousHistEntry();

	printf("%s\n", getCurrentEntry()); //01
	moveToPreviousHistEntry();

	printf("%s\n", getCurrentEntry());  //03
	moveToPreviousHistEntry();

	printf("%s\n", getCurrentEntry()); //02
	moveToPreviousHistEntry();

	printf("%s\n", getCurrentEntry()); //01


	moveToNextHistEntry();
 	printf("%s\n", getCurrentEntry()); //02

	moveToNextHistEntry();
 	printf("%s\n", getCurrentEntry()); //03

	moveToNextHistEntry();
 	printf("%s\n", getCurrentEntry()); //null

	moveToNextHistEntry();
 	printf("%s\n", getCurrentEntry()); //null

	moveToPreviousHistEntry();
	printf("%s\n", getCurrentEntry()); //03

	printf("%s %d\n", getCurrentHistEntry(), getCurrentHistPos()); //03
	printf("%s\n", getCurrentHistEntry()); //02
	printf("%s\n", getCurrentHistEntry()); //01
	printf("%s %d\n", getCurrentHistEntry(), getCurrentHistPos()); //03
	printf("%s\n", getCurrentHistEntry()); //02
	printf("%s\n", getCurrentHistEntry()); //01


	printf("%s\n", moveToNextHistEntry()); //03
	printf("%s\n", moveToNextHistEntry()); //null
	printf("%s %d\n", moveToNextHistEntry(), getCurrentHistPos()); //null 3
	printf("%s\n", moveToNextHistEntry()); //null
	printf("%s\n", moveToNextHistEntry()); //null

	printf("%s\n", moveToPreviousHistEntry()); //03*/

	/*while(1)
	{
		char choix = wait_one_key();
		if(choix =='h')
		{

			printf("%s\n", moveToPreviousHistEntry());
		}
		if(choix =='b')
		{

		 	printf("%s\n", moveToNextHistEntry());
		}
		if(choix == 'a')
			return;
	}*/

}

void move_to_next_history_TEST()
{
	addHistoryEntry("01");
	addHistoryEntry("02");
	addHistoryEntry("03");

	printf("%s\n", getCurrentEntry()); //01


	moveToNextHistEntry();
 	printf("%s\n", getCurrentEntry()); //02

	moveToNextHistEntry();
 	printf("%s\n", getCurrentEntry()); //03

	moveToNextHistEntry();
 	printf("%s\n", getCurrentEntry()); //null

	moveToNextHistEntry();
 	printf("%s\n", getCurrentEntry()); //null

	moveToPreviousHistEntry();
	printf("%s\n", getCurrentEntry()); //03


}

void move_to_previous_history_TEST()
{
	addHistoryEntry("01");
	addHistoryEntry("02");
	addHistoryEntry("03");

	printf("%s\n", getCurrentEntry()); //03
	moveToPreviousHistEntry();

	printf("%s\n", getCurrentEntry()); //02
	moveToPreviousHistEntry();

	printf("%s\n", getCurrentEntry()); //01
	moveToPreviousHistEntry();

	printf("%s\n", getCurrentEntry());  //03
	moveToPreviousHistEntry();

	printf("%s\n", getCurrentEntry()); //02
	moveToPreviousHistEntry();

	printf("%s\n", getCurrentEntry()); //01

}

void wait_one_key_TEST()
{
	Key k = wait_one_key();
	printf("Got %c %d\n", k.c, k.isSpecialKey);
}

/*void read_key_TEST()
{

	Line* buf = malloc(sizeof(Line*));
		buf->text = malloc(200);
		strcpy(buf->text, "hello ");
		buf->pos = 2;
		buf->length = 6;
	refreshTerminal(buf);
	while(1)
	{
		Key k = readkey(buf);
		if(k.c == 'x')
			return;
		refreshTerminal(buf);
	}
}*/

void insert_into_string_TEST()
{
	char* str = malloc(sizeof(char*));
	strcpy(str, "hello");
	insert_into_string(str, "k", 6);
	printf("%s", str); //hkello

}

void insert_delete_of_string_TEST()
{
	char* str = malloc(sizeof(char*));
	strcpy(str, "hello");

	insert_into_string(str, "k", 0);
	printf("%s\n", str); //khello

	delete_from_string(str, 0);
	printf("%s\n", str); //hello

	insert_into_string(str, "k", 2);
	printf("%s\n", str); //hekllo

	delete_from_string(str, 2);
	printf("%s\n", str); //hello

	delete_from_string(str, 2);
	printf("%s\n", str); //helo

	delete_from_string(str, 2);
	printf("%s\n", str); //heo

}


void string_delete_TEST()
{
	Line* buf = malloc(sizeof(Line*));
	buf->text = malloc(200);
	strcpy(buf->text, "hello ");
	buf->pos = 2;

	memmove(&buf->text[buf->pos], &buf->text[buf->pos + 1], strlen(buf->text) - buf->pos);
	printf("%s\n", buf->text); //helo

}

