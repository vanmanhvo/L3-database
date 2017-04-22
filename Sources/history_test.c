/*
 * history.c
 *
 *  Created on: 21 avr. 2017
 *      Author: vovanmanh
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Headers/history_test.h"

/*#define HISTORY_SIZE 100;

char* history[HISTORY_SIZE];


int nextHistPos = 0;
int currentHistPos = -1;

int getCurrentHistPos()
{
	return currentHistPos;
}


void addHistoryEntry (const char * entry)
{
	history[nextHistPos % HISTORY_SIZE] = malloc(sizeof(char*));
	strcpy(history[nextHistPos], entry);
	currentHistPos = nextHistPos;
	nextHistPos++;
}

char* getCurrentEntry()
{
	if(currentHistPos >= 0 && currentHistPos < nextHistPos)
		return history[currentHistPos];
	else
		return NULL;
}


void moveToPreviousHistEntry()
{
	if(nextHistPos == 0)
		return;
	currentHistPos--;

	if(currentHistPos < 0)
	{
		currentHistPos = nextHistPos-1;
	}

	if(currentHistPos < 0)
	{
		currentHistPos = nextHistPos-1;
		return history[currentHistPos];
	}
	if(currentHistPos < 0)
		return history[0];
	//if(currentHistPos == nextHistPos-1)
		//currentHistPos--;*/
//}


/*void moveToNextHistEntry()
{
	if(nextHistPos == 0)
		return;
	if(currentHistPos >= nextHistPos)
		return;*/
	/*if(currentHistPos < 0)
		currentHistPos = 0;*/
	//currentHistPos++;
//}
