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
#include "../Headers/outils.h"

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

void editOrAddHistoryEntry(const char * entry)
{
	if (isEmptyOrSpace(entry)) {
		return;
	}
	if (currentHistPos<HISTORY_SIZE)
		strcpy(history[currentHistPos], entry);
	if (currentHistPos == nextHistPos && nextHistPos<HISTORY_SIZE) {
		nextHistPos++; //add a new one
	}
}

void newHistoryEntry(const char * entry)
{
	if (nextHistPos<HISTORY_SIZE) {
		strcpy(history[nextHistPos], entry);
		nextHistPos++;
		currentHistPos = nextHistPos;
	}
}

char* moveToPreviousHistEntry()
{
	if(nextHistPos == 0) { //history is empty
		currentHistPos = 0;
		return NULL;
	}
	if(currentHistPos == 0) {
		return history[0];
	}
	return history[--currentHistPos];
}

char* moveToNextHistEntry()
{
	if(nextHistPos == 0) { //history is empty
		currentHistPos = 0;
		return NULL;
	}
	if(currentHistPos >= nextHistPos)
		return NULL;
	return history[++currentHistPos];
}

