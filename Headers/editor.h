/*
 * editor.h
 *
 *  Created on: 22 avr. 2017
 *      Author: vovanmanh
 */

#include "typedef.h"


#ifndef HEADERS_EDITOR_H_
#define HEADERS_EDITOR_H_

typedef struct Line
{
	char* text;
	int pos;
	//int length;
}Line;

void clearLine();

Key readkey(Line* buf);

void refreshTerminal(Line* buf);

char* readLine(Line* buf);


#endif /* HEADERS_EDITOR_H_ */
