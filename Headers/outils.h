/*
 * outils.h
 *
 *  Created on: 10 avr. 2017
 *      Author: vovanmanh
 */

#include "typedef.h"


#ifndef HEADERS_OUTILS_H_
#define HEADERS_OUTILS_H_



void logTime(char *q);
void logQuery(char *q);
void Write_to_file(char *q, char* filename);
void save_history (const char * cmd);
Key wait_one_key ();
void insert_into_string (char *subject, const char* insert, int pos);
void delete_from_string(char* s, int pos);



char* racourcir(char* query);
void mode_raw(int activer);
void lignePrecedente(FILE *fichier);
char isUp();

#endif /* HEADERS_OUTILS_H_ */
