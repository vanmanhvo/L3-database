/*
 * parser.h
 *
 *  Created on: 9 août 2016
 *      Author: Andy
 */




#ifndef HEADERS_PARSER_H_
#define HEADERS_PARSER_H_

#include "tableOperations.h"







void sql(char *command);
void initQuery(query *q, queryType type);
char* majuscule(char *chaine);
void createTable(char *str);
void deleteFromTable(char *str);
void insertIntoTable(char *str);
void selectFrom(char *str);
void updateTable(char *str);

#endif /* HEADERS_PARSER_H_ */
