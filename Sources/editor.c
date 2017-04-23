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

		editOrAddHistoryEntry(buf->text); //edit current history entry or add a new one
	}
	if(k.isSpecialKey == 1)
	{
		if(k.c == 'l' && buf->pos > 0) //left
			buf->pos--;
		if(k.c == 'r' && buf->pos < strlen(buf->text)) //right
			buf->pos++;
		if(k.c == 'b' && buf->pos > 0) //back
		{
			buf->pos--;
			delete_from_string(buf->text, buf->pos);
			editOrAddHistoryEntry(buf->text);
		}
		if(k.c == 'u') //up
		{
			char* entry = moveToPreviousHistEntry();
			if(isEmpty(entry) == 0)
			{
				strcpy(buf->text, entry);
				buf->pos = strlen(entry);
			}
		}
		if(k.c == 'd') //down
		{
			char* entry = moveToNextHistEntry();
			if(isEmpty(entry) == 1)
			{
				strcpy(buf->text, "");
				buf->pos = 0;
			}
			else
			{
				strcpy(buf->text, entry);
				buf->pos = strlen(entry);
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

char* readLine(Line* buf)
{
	refreshTerminal(buf);
	editOrAddHistoryEntry(buf->text);
	char* result = malloc(256);
	while(1)
	{
		Key k = readkey(buf);
		refreshTerminal(buf);

		if(k.c == 'e' && k.isSpecialKey == 1)
		{
			memcpy(result, buf->text, strlen(buf->text));
			clearLine();
			printf("bql > %s\n", result);
			return result;
		}
		if(k.c == 'q' && k.isSpecialKey == 1)
		{
			exit(0);
		}
	}
}

/**
 * an advance fgets(), use to read the terminal stdin by editing a pre-defined content
 */
char* editLine(char* content)
{
	Line buf;
	strcpy(buf.text, isEmpty(content)==1 ? "" : content);
	char* cmd=readLine(&buf);
	moveToNextHistEntry();
	return cmd;
}

