/*
 * editor.c
 *
 *  Created on: 22 avr. 2017
 *      Author: vovanmanh
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../Headers/editor.h"
#include "../Headers/outils.h"
#include "../Headers/history.h"



void clearLine()
{
	printf("\x0d");
	printf("%c[2K", 27);
	//printf("\033[A\033[K");
	//"ESC[K";
}

Key readkey(Line* buf)
{
	Key k = wait_one_key();
	if(k.isSpecialKey == 0)
	{
		char x[2];
		x[0] = k.c;
		x[1] = '\0';
		insert_into_string(buf->text, x, buf->pos);
		buf->pos++;
		//strcat(buf->text, "\0");
	}
	if(k.isSpecialKey == 1)
	{
		if(k.c == 'l' && buf->pos > 0)
			buf->pos--;
		if(k.c == 'r' && buf->pos < strlen(buf->text))
			buf->pos++;
		if(k.c == 'b' && buf->pos > 0)
		{
			buf->pos--;
			delete_from_string(buf->text, buf->pos);
		}
		if(k.c == 'u')
		{
			char* entry = moveToPreviousHistEntry();
			if(entry != NULL)
			{
				strcpy(buf->text, entry);
				//strcat(buf->text, " ");
				buf->pos = strlen(buf->text)-1;
			}
		}
		if(k.c == 'd')
		{
			char* entry = moveToNextHistEntry();
			if(entry == NULL)
			{
				buf->text = " ";
				buf->pos = 0;
			}
			else
			{
				strcpy(buf->text, entry);
				//strcat(buf->text, " ");
				buf->pos = strlen(buf->text)-1;
			}
		}
	}
	return k;
}

//Print a buffer text to terminal and move cursor to the buffer position
void refreshTerminal(Line* buf)
{
	clearLine();
	printf("bql > %s", buf->text);
	int n = strlen(buf->text)-buf->pos;
	if(n > 0)
		printf("\033[%dD", n); // Move left X column;
}

//

char* readLine(Line* buf)
{
	refreshTerminal(buf);
	char* result = malloc(200);
	while(1)
	{
		Key k = readkey(buf);
		refreshTerminal(buf);
		//editHistoryEntry(buf->text);
		if(k.c == 'e' && k.isSpecialKey == 1)
		{
			memcpy(result, buf->text, strlen(buf->text)-1);
			clearLine();
			printf("bql > %s\n", result);
			//addHistoryEntry(result);
			return result;
		}
		if(k.c == 'q' && k.isSpecialKey == 1)
		{
			exit(0);
		}
	}
}
