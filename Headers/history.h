/*
 * history.h
 *
 *  Created on: 10 avr. 2017
 *      Author: vovanmanh
 */

#ifndef HEADERS_HISTORY_H_
#define HEADERS_HISTORY_H_

#define HISTORY_SIZE 100

void addHistoryEntry (const char * entry);

void editHistoryEntry (const char * entry);


char* moveToPreviousHistEntry();
char* moveToNextHistEntry();

char* getCurrentEntry();


int getCurrentHistPos();



#endif /* HEADERS_OUTILS_H_ */
