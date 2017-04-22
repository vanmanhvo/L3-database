/*
 * page.h
 *
 *  Created on: 10 juil. 2016
 *      Author: Andy
 */


#ifndef page_H_
#define page_H_


//Structures
//Les tailles ne sont pas exactes, elles sont fixés seulement pour les tests pour l'instant

typedef enum {
    CHAR,
    INT,
    FLOAT,
    DOUBLE,
	DATE,
	//TIME,
	//TIMESTAMP,
	BIGINT,
	BLOB,
	ACSII,
	TEXT,
	VARINT,
	UUID,
	TIMEUUID,
	BOOLEAN,
	DECIMAL,
	COUNTER
} TYPE;

typedef enum {
	PRIMARY_KEY,
	NOT_NULL,
	DEFAULT,
	UNIQUE,
	FOREIGN_KEY,
	CHECK,
	INDEX
} CONSTRAINT;



typedef enum {
	CREATE_KEYSPACE,
	CREATE_TABLE,
	ALTER,
	INSERT,
	DELETE,
	UPDATE,
	SELECT
} queryType;


typedef struct clauseWhere {
	int nbConditions;
	char *tabNomColonne[10];
	char *tabOperateur[10];
	char *tabValeur[10];
	char *tabDelim[9];
} clauseWhere;

typedef struct clauseJoin {
	char *nomTable;
	clauseWhere on;
} clauseJoin;

typedef struct {
	char nom[40];
	TYPE type;
	int size;
	int nb;
	CONSTRAINT *constraint;
	int nbConstraint;
	void *defaultValue;
} colonne;

typedef struct query {
	queryType type;
	char *nomTable;
	colonne tabColonne[100];
	int nbColonnes;
	char *tabValeurs[100];
	int nbValeurs;
	clauseWhere where;
	int nbJoin;
	clauseJoin join[10];
} query;


typedef struct {
	char nom[20];
	colonne listeColonnes[100];
	int nbColonnes;
	int tailleElem;
} table;

typedef struct {
	table t;
} headerFP;

typedef struct {
	int nbElements;
} header;

typedef struct  {
	header h;
	unsigned char data[8188];
} page;

typedef struct {
	headerFP h;
} firstPage;

typedef struct {
	firstPage fp;
	page p[7];
	int id;
	int altered;
} extension;






void creerExtension(extension *pE, char *nomTable);
void creerTable(query *q);
void trouverPlace(char *nomTable, int *numBloc, int *numExt);
void insererValeurs(query *q);
void majTable(query *q);
void supprimerElementTableWhere(query *q);
void selectionner(query *q);
void afficherValeur(extension *pE, colonne c, void *row, int decCol);


#endif /* page_H_ */
