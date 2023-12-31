
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Code permettant de stocker différents types de couleurs : format 24 bits (RGB) ou format 32 bits (32). 
 */

#ifndef __JSON_H__
#define __JSON_H__

typedef struct {
    char* code;
    char* valeurs;
} TableauDeChaines;


TableauDeChaines extraireCodeEtValeurs(const char* jsonString);
void formaterMessage(const char *entree, char *sortie);
void formatJsonForList(char *chaine);
void formatStringForJson(char *chaine);



#endif