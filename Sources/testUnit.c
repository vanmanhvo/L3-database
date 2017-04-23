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

