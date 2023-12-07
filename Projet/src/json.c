/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
/*
 * Code pour le traitement des messages au format JSON
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include "json.h"


// Fonction qui vérifie si une chaîne est un nombre
int isNumber(const char *str) {
    char *endptr;
    strtod(str, &endptr);
    return *endptr == '\0';
}

// Fonction qui permet de formater une chaîne en message JSON
void formaterMessage(const char *entree, char *sortie) {
    const char *delims = ":, ";
    char *token, *copy;
    char *code = NULL;
    char *valeurs = NULL;
    size_t valeurs_len = 0;

    // On copie l'entrée pour la modification
    copy = strdup(entree);

    // On utilise strtok pour extraire le code
    token = strtok(copy, delims);
    if (token == NULL) {
        snprintf(sortie, 1024, "{\"erreur\": \"Format invalide pour l'entrée.\"}");
        free(copy);
        return;
    }
    code = strdup(token);

    // On utilise strtok pour extraire les valeurs
    token = strtok(NULL, delims);
    if (token == NULL) {
        snprintf(sortie, 1024, "{\"erreur\": \"Format invalide pour l'entrée.\"}");
        free(copy);
        free(code);
        return;
    }

    // On ajoute la première valeur 
    valeurs = realloc(valeurs, valeurs_len + strlen(token) + 3);
    memset(valeurs,0,valeurs_len + strlen(token) + 3);
    if (valeurs == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        exit(EXIT_FAILURE);
    }
    

    // Si le token est un nombre, on ne l'entoure pas de guillemets
    if (isNumber(token)) {
        strcat(valeurs, token);
    } else {
        strcat(valeurs, "\"");
        strcat(valeurs, token);
        strcat(valeurs, "\"");
    }

    valeurs_len += strlen(token) + 2;

    // On vérifie s'il y a d'autres valeurs séparées par des virgules ou des espaces
    while ((token = strtok(NULL, delims)) != NULL) {
        // On alloue de l'espace pour la nouvelle valeur et la virgule
        valeurs = realloc(valeurs, valeurs_len + strlen(token) + 5);
        if (valeurs == NULL) {
            fprintf(stderr, "Erreur d'allocation mémoire.\n");
            exit(EXIT_FAILURE);
        }

        // Si le token est un nombre, on ne l'entoure pas de guillemets
        if (isNumber(token)) {
            strcat(valeurs, ", ");
            strcat(valeurs, token);
        } else {
            strcat(valeurs, ", \"");
            strcat(valeurs, token);
            strcat(valeurs, "\"");
        }

        valeurs_len += strlen(token) + 4;
    }

    
    // Formate la sortie
    snprintf(sortie, 1024, "{\n\t\"code\" : \"%s\",\n\t\"valeurs\" : [ %s ]\n}", code, valeurs);

    // On libère la mémoire allouée
    free(copy);
    free(code);
    free(valeurs);
}

// fonction pour formater un json simple avec une seul valeur
void formatStringForJson(char *chaine) {
    // On initialise un index pour la nouvelle chaîne
    int nouvelIndex = 0;


    // On parcour chaque caractère de la chaîne
    for (int i = 0; i < strlen(chaine); i++) {
        // Ignorer les espaces, guillemets et virgules
        if (chaine[i] != '"' && chaine[i] != ',') {
            // On ajoute le caractère à la nouvelle chaîne
            chaine[nouvelIndex++] = chaine[i];
        }
    }

    // On ajoute le caractère nul à la fin de la nouvelle chaîne
    chaine[nouvelIndex] = '\0';
}

// fonction pour formater un json contenant une liste
void formatJsonForList(char *chaine) {
    // On initialise un index pour la nouvelle chaîne
    int nouvelIndex = 0;


    // On parcour chaque caractère de la chaîne
    for (int i = 0; i < strlen(chaine); i++) {
        // Ignorer les espaces, guillemets et virgules
        if (chaine[i] != '"' && chaine[i] != ' ') {
            // Ajouter le caractère à la nouvelle chaîne
            chaine[nouvelIndex++] = chaine[i];
        }
    }

    // On ajoute le caractère nul à la fin de la nouvelle chaîne
    chaine[nouvelIndex] = '\0';
}

// Fonction qui retourne un tableau de deux chaînes de caractères
TableauDeChaines extraireCodeEtValeurs(const char* jsonString) {
    TableauDeChaines result;

    result.code = NULL;
    result.valeurs = NULL;

    const char* codeDebut = strstr(jsonString, "\"code\"");
    if (codeDebut == NULL) {
        fprintf(stderr, "Le champ 'code' est introuvable dans le JSON.\n");
        return result;  // Pas besoin de libérer la mémoire, car elle n'a pas été allouée
    }

    codeDebut = strchr(codeDebut, ':');
    if (codeDebut == NULL) {
        fprintf(stderr, "Format JSON invalide.\n");
        return result;
    }
    codeDebut++;

    while (*codeDebut == ' ' || *codeDebut == '\t' || *codeDebut == '\n' || *codeDebut == '\r') {
        codeDebut++;
    }

    if (*codeDebut != '\"') {
        fprintf(stderr, "Format JSON invalide pour la valeur associée au champ 'code'.\n");
        return result;
    }

    codeDebut++;

    const char* codeFin = strchr(codeDebut, '"');
    if (codeFin == NULL) {
        fprintf(stderr, "Format JSON invalide.\n");
        return result;
    }

    size_t longueurCode = codeFin - codeDebut;

    result.code = malloc(longueurCode + 1);
    if (result.code == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        return result;
    }

    strncpy(result.code, codeDebut, longueurCode);
    result.code[longueurCode] = '\0';

    const char* valeursDebut = strstr(jsonString, "\"valeurs\"");
    if (valeursDebut == NULL) {
        fprintf(stderr, "Le champ 'valeurs' est introuvable dans le JSON.\n");
        free(result.code);
        return result;
    }

    valeursDebut = strchr(valeursDebut, ':');
    if (valeursDebut == NULL) {
        fprintf(stderr, "Format JSON invalide.\n");
        free(result.code);
        return result;
    }
    valeursDebut++;

    while (*valeursDebut == ' ' || *valeursDebut == '\t' || *valeursDebut == '\n' || *valeursDebut == '\r') {
        valeursDebut++;
    }

    if (*valeursDebut != '[') {
        fprintf(stderr, "Format JSON invalide pour le tableau de valeurs.\n");
        free(result.code);
        return result;
    }

    valeursDebut++;

    const char* valeursFin = strchr(valeursDebut, ']');
    if (valeursFin == NULL) {
        fprintf(stderr, "Format JSON invalide pour le tableau de valeurs.\n");
        free(result.code);
        return result;
    }

    size_t longueurValeurs = valeursFin - valeursDebut;
    result.valeurs = malloc(longueurValeurs + 1);
    if (result.valeurs == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        free(result.code);
        return result;
    }

    strncpy(result.valeurs, valeursDebut, longueurValeurs);
    result.valeurs[longueurValeurs] = '\0';

    return result;
}

