/*
 * timestamp.h
 *
 *  Created on: 20 avr. 2017
 *      Author: vovanmanh
 */

#ifndef HEADERS_TIMESTAMP_H_
#define HEADERS_TIMESTAMP_H_

/*struct tm
{
    int tm_sec;        secondes (0,59)
    int tm_min;        minutes (0,59)
    int tm_hour;       heures depuis minuit (0,23)
    int tm_mday;       jour du mois (0,31)
    int tm_mon;        mois depuis janvier (0,11)
    int tm_year;       années écoulées depuis 1900
    int tm_wday;       jour depuis dimanche (0,6)
    int tm_tm_yday;    jour depuis le 1er janvier (0,365)
    int tm_isdst;
};*/

const char * NomJourSemaine[] = {"Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi"};

const char * NomMois[] = {"janvier", "fevrier", "mars"     , "avril"  , "mai"     , "juin"    ,
                          "juillet", "aout"   , "septembre", "octobre", "novembre", "decembre"};

#endif /* HEADERS_TIMESTAMP_H_ */
