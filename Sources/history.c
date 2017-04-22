/*
 * history.c
 *
 *  Created on: 21 avr. 2017
 *      Author: vovanmanh
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Headers/history.h"

char history[HISTORY_SIZE][200];


int nextHistPos = 0;
int currentHistPos = 0;

int getCurrentHistPos()
{
	return currentHistPos;
}

char* getCurrentEntry()
{
	if(currentHistPos >= 0 && currentHistPos < nextHistPos)
		return history[currentHistPos];
	else
		return NULL;
}

void addHistoryEntry (const char * entry)
{
	strcpy(history[nextHistPos], entry);
	currentHistPos = ++nextHistPos;


}

void editHistoryEntry (const char * entry)
{
	strcpy(history[currentHistPos], entry);
	//currentHistPos = nextHistPos;
	//nextHistPos++;
}

char* moveToPreviousHistEntry()
{
	if(nextHistPos == 0)
		return NULL;
	if(currentHistPos == 0)
		return history[0];
	return history[--currentHistPos];
}

char* moveToNextHistEntry()
{
	if(nextHistPos == 0)
		return NULL;
	if(currentHistPos >= nextHistPos)
		return NULL;
	return history[++currentHistPos];
}

