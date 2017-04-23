/*
 * outils.c
 *
 *  Created on: 10 avr. 2017
 *      Author: vovanmanh
 */


#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#include "../Headers/typedef.h"


#define TAILLE_MAX 1000

// inserts into subject[] at position pos
void insert_into_string (char *subject, const char* insert, int pos) {
    char buf[100] = {}; // 100 so that it's big enough. fill with zeros
    // or you could use malloc() to allocate sufficient space
    // e.g. char *buf = (char*)malloc(strlen(subject) + strlen(insert) + 2);
    // to fill with zeros: memset(buf, 0, 100);

    strncpy(buf, subject, pos); // copy at most first pos characters
    int len = strlen(buf);
    strcpy(buf+len, insert); // copy all of insert[] at the end
    len += strlen(insert);  // increase the length by length of insert[]
    strcpy(buf+len, subject+pos); // copy the rest

    strcpy(subject, buf);   // copy it back to subject
    // Note that subject[] must be big enough, or else segfault.
    // deallocate buf[] here, if used malloc()
    // e.g. free(buf);
}

void delete_from_string(char* s, int pos)
{
	memmove(&s[pos], &s[pos + 1], strlen(s) - pos);
}

//Write a string to the file log.txt
void Write_to_file(const char *q, const char* filename)
{
	FILE *log = NULL;
	log = fopen(filename, "a");
	if(log != NULL)
	{
		fprintf(log, "%s", q);
		fclose(log);
	}
	else
		printf("erreur d'écriture log");
}

void logQuery(const char *q)
{
	char* c = malloc(sizeof(char*));
	strcpy(c, q);
	strcat(c, ";\n");
	Write_to_file(c, "log.txt");
}


void logTime(const char *q)
{
	char* r = malloc(sizeof(char*));
	sprintf(r, "\n-------------------------\n%s-------------------------\n", q);
	Write_to_file(r, "log.txt");
}



void lignePrecedente (FILE * fichier)
{
  long p0, p1, p2, i;
  char c = 'c';
  p2 = ftell (fichier);     //position actuelle
  //printf ("position actuelle %ld\n", p2);
  fseek (fichier, 0, SEEK_SET); //place curseur au debut du fichier
  p1 = ftell (fichier);     //position actuelle
  i = p1;
  p0 = p1;
  while (i < p2 && c != EOF)
    {
      c = fgetc (fichier);
      i = ftell (fichier);
      //printf ("%ld ", i);
      if (c == '\n')
    {
      p0 = p1;
      p1 = i;
    }
    }
  //printf
    //("\nLe debut de la ligne precedente est a %ld caractere par rapport a la position initial\n",
     //p0 - i);
  fseek (fichier, p0 - i, SEEK_CUR);    //on place le curseur au debut de la ligne precedente
}

int compare(const char* chaine1, const char* chaine2)
{   unsigned int i=0;
    if( strlen(chaine1) != strlen(chaine2) )
        return 0;
    for(i=0;i<strlen(chaine1);i++)
        if( chaine1[i] != chaine2[i])
            return 0;
    return 1;
}


/*
 * Fonction utilitaire pour retirer les \n de la requête sql
 */
void replace(char *s, char ch, char repl) {

	//Retire les \n de la requete sql
	for(int i=0;i<(int)strlen(s);i++)
	{
		if(s[i] == (char)ch)
			s[i] = repl;
	}

	//Retire les espaces présents à la fin de la requête si il y en a
	while(s[strlen(s)-1] == ' ')
	{
		s[strlen(s)-1] = '\0';
	}

}

/*
 * Passer la console en mode RAW ou COOCKED
 */
void mode_raw(int activer)
{
    static struct termios cooked;
    static int raw_actif = 0;

    if (raw_actif == activer)
        return;

    if (activer)
    {
        struct termios raw;

        tcgetattr(STDIN_FILENO, &cooked);

        raw = cooked;
        cfmakeraw(&raw);
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }
    else
        tcsetattr(STDIN_FILENO, TCSANOW, &cooked);

    raw_actif = activer;
}

char isUp(char choix)
{
	  int i = 0;
	  char c[3];
	  //FILE* fichier = NULL;
	  //int true = 1;
	  //char *requete; // Chaîne vide de taille TAILLE_MAX


	  //while(1){

	  while(i<3)
	  {
		  mode_raw(1);
		  c[i] = (int)getchar();
		  i++;
	  //}
		  }
	  mode_raw(0);

	  //i = 0;


	  if(c[0] == 27 && c[1] == 91 && c[2] == 65)
	  {
		  //lignePrecedente(fichier);
		  //fgets(requete, TAILLE_MAX, fichier);
		  //printf("%s", requete);
		  //fclose(fichier);
		  //requete = "up";
		  //true = 1;
		  choix = 'u';
	  }

	  else if(c[0] == 27 && c[1] == 91 && c[2] == 66)
	  {

		  //fgets(requete, TAILLE_MAX, fichier);
		  //printf("down");
		  //fclose(fichier);
		  //true = 0;
		  choix = 'd';
	  }
	  return choix;

}

void cexit(void)
{

	mode_raw(0);
	putc('\n',stdout);
	exit(0);
}

//This function wait for user to click on a key.
//if the key is an arrow key, it return the name of the arrow key
//otherwise it return 'a'
Key wait_one_key ()
{
	int c = 0;
	Key key;

	mode_raw(1);
	c = getchar();
	key.c = c;
	key.isSpecialKey = 0;

	if (c==127) //Backspace
	{
		key.c = 'b';
		key.isSpecialKey = 1;
	}

	if(c==17) //CTRL+q
	{
		key.c = 'q';
		key.isSpecialKey = 1;
	}

	if(c==13) //Enter
	{
		key.c = 'e';
		key.isSpecialKey = 1;
	}


	if(c==27) {
		c=getchar();
		if (c==91)
		{
			c=getchar();

			switch(c)
			{
				case 'A': /*fleche du haut*/
					key.c = 'u';
					key.isSpecialKey = 1;
					//printf("%s", result);
					break;
				case 'B': /*fleche du bas*/
					key.c = 'd';
					key.isSpecialKey = 1;
					//printf("%s", result);
					break;
				case 'C': /*fleche de droite*/
					key.c = 'r';
					key.isSpecialKey = 1;
					//printf("%s", result);
					break;
				case 'D': /*fleche de gauche*/
					key.c = 'l';
					key.isSpecialKey = 1;
					//printf("%s", result);
					break;
			}


			if(c==51)
			{
				c=getchar();
				if(c==126) //delete
				{
					key.c = 'x';
					key.isSpecialKey = 1;
				}

			}
		}


		if (c==91)
		{
			c=getchar();
		}
	}
	mode_raw(0);
	return key;
}

int isEmpty(char* s) {
	if (s==NULL || s[0]=='\0' || s[0]==EOF)
		return 1;
	return 0;
}

int isEmptyOrSpace(char* s) {
	if (s==NULL || s[0]=='\0' || s[0]==EOF || strcmp(s, " ")==0)
		return 1;
	return 0;
}

/*
 *Concatener deux chaines de caractères
 */

char *concat_string(const char *s1, const char *s2)
{
     char *s3=NULL;
     s3=(char *)malloc((strlen(s1)+strlen(s2))+1);
     strcpy(s3,s1);
     strcat(s3,s2);
     return s3;
}


//Read line of a file
/*char *readHistoryEntry (FILE* file, int indexEntry)
{

}
*/
