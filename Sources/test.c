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

void run()
{
	//mode_raw(1);
	file = creerFile(6000);
	hash = creerHash(300);

	startDb();
}


void execute(char* command) {
	printf("Got '%s'\n", command);
}

void manh() {
	printf("\nWelcome to L3 database application\n");
	printf("type Ctrl+Q or enter the command 'exit' to quit\n\n");
	while(1) {
		char* command = editLine("");

		if (strcmp(command, "exit")==0)
			break;

		execute(command);
	}
}

int main()
{
	manh();
	return 0;
}

