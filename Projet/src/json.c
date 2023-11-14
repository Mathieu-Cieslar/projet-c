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
#include "json.h"

//fonction qui permet de formater une chaine en message json
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
        snprintf(sortie, 200, "{\"erreur\": \"Format invalide pour l'entrée.\"}");
        free(copy);
        return;
    }
    code = strdup(token);

    // On utilise strtok pour extraire les valeurs
    token = strtok(NULL, delims);
    if (token == NULL) {
        snprintf(sortie, 200, "{\"erreur\": \"Format invalide pour l'entrée.\"}");
        free(copy);
        free(code);
        return;
    }

    // On ajoute la première valeur (opérateur ou chiffre)
    valeurs = realloc(valeurs, valeurs_len + strlen(token) + 3);
    if (valeurs == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        exit(EXIT_FAILURE);
    }
    strcat(valeurs, "\"");
    strcat(valeurs, token);
    strcat(valeurs, "\"");
    valeurs_len += strlen(token) + 2;

    // On verifie s'il y a d'autres valeurs séparées par des virgules ou des espaces
    while ((token = strtok(NULL, delims)) != NULL) {
        // On alloue de l'espace pour la nouvelle valeur et la virgule
        valeurs = realloc(valeurs, valeurs_len + strlen(token) + 5);
        if (valeurs == NULL) {
            fprintf(stderr, "Erreur d'allocation mémoire.\n");
            exit(EXIT_FAILURE);
        }
        strcat(valeurs, ", \"");
        strcat(valeurs, token);
        strcat(valeurs, "\"");
        valeurs_len += strlen(token) + 4;
    }

    // Formate la sortie
    snprintf(sortie, 200, "{\n\t\"code\" : \"%s\",\n\t\"valeurs\" : [ %s ]\n}", code, valeurs);

    // On libère la mémoire allouée
    free(copy);
    free(code);
    free(valeurs);
}
