/*
 * test.c
 *
 *  Created on: 9 juil. 2016
 *      Author: Andy
 */

//Includes
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

void read_key_TEST()
{

	Line* buf = malloc(sizeof(Line*));
		buf->text = malloc(200);
		strcpy(buf->text, "hello ");
		buf->pos = 2;
	refreshTerminal(buf);
	while(1)
	{
		Key k = readkey(buf);
		if(k.c == 'x')
			return;
		refreshTerminal(buf);
	}
}

void test_history()
{
	addHistoryEntry("00");
	addHistoryEntry("01");
	addHistoryEntry("02");

	printf("%s\n", moveToPreviousHistEntry()); //02
	printf("%s\n", moveToPreviousHistEntry()); //01
	printf("%s\n", moveToPreviousHistEntry()); //00
	printf("%s\n", moveToPreviousHistEntry()); //00
	printf("%s\n", moveToPreviousHistEntry()); //00
	editHistoryEntry("00A");

	printf("%s\n", moveToNextHistEntry()); //01
	printf("%s\n", moveToNextHistEntry()); //02
	printf("%s\n", moveToNextHistEntry()); //null
	printf("%s\n", moveToNextHistEntry()); //null


	printf("%s\n", moveToPreviousHistEntry()); //02
	printf("%s\n", moveToPreviousHistEntry()); //01
	printf("%s\n", moveToPreviousHistEntry()); //00

	printf("%s\n", moveToNextHistEntry()); //01


}

void test_read_line()
{


	//refreshTerminal(buf);

	/*while(1)
	{
		Line* buf = malloc(sizeof(Line*));
		buf->text = malloc(sizeof(char)*200);
		strcpy(buf->text, " ");
		buf->length = 1;
		char* cmd= readLine(buf);
		printf("%s\n", cmd);
		if(strcmp(cmd, "exit") == 0)
			break;
	}*/
	Line* buf = malloc(sizeof(Line*));
	buf->text = malloc(sizeof(char)*200);
	strcpy(buf->text, "hi ");
	char* cmd= readLine(buf);
	printf("%s\n", buf->text);
}

void run()
{
	//mode_raw(1);
	file = creerFile(6000);
	hash = creerHash(300);

	startDb();

}

void readLine_TEST ()
{
	Line* buf = malloc(sizeof(Line*));
	buf->text = malloc(200);
	strcpy(buf->text, "hello ");
	buf->pos = 2;

	refreshTerminal(buf);
	char * line = readLine(buf);

	printf("%s\n", line);
}


int main()
{
	test_read_line();
	//test_history();
	//readLine_TEST();
	//read_key_TEST();
	//run();
	//insert_delete_of_string_test();
	//test_string_delete();
	//insert_into_string_test();
	//read_key_test();
	//wait_one_key_test();
	/*printf("\nhello world");
	char choix = wait_one_key();
	if(choix =='h')
	{
		clearLine();
		printf("hello world manh\n");
	}*/

	/*Line* buf = malloc(sizeof(Line*));
	buf->text = "hello";
	buf->pos = 2;
	refreshTerminal(buf);*/



	//testHist();
	//mode_raw(1);
	//run();
	//printf("%c\n", wait_one_key());

	//printf("%c", isUp());
	//printf("finish\n");

	return 0;
}

