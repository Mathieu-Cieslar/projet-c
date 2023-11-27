
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* 
 * Le but principal de ce code est de s'assurer que les messages envoyés par le client sont
 * valides, c'est-à-dire que les messages respectent le format JSON et
 * respectent le protocole spécifié par le côté serveur.
 */

#include <math.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>

int validationJSON(char data[1024]){
regex_t regex;
    int reti;

    // Expression régulière pour vérifier la structure JSON
    const char *pattern = "\\{\\s*\"code\"\\s*:\\s*\"[^\"]+\"\\s*,\\s*\"valeurs\"\\s*:\\s*\\[.*\\]\\s*\\}";

    // Compiler l'expression régulière
    reti = regcomp(&regex, pattern, REG_EXTENDED);
    if (reti) {
        fprintf(stderr, "Erreur lors de la compilation de l'expression régulière\n");
        return 0;  // Retourner 0 en cas d'erreur
    }

    // Exécuter la correspondance
    reti = regexec(&regex, data, 0, NULL, 0);
    regfree(&regex);  // Libérer la mémoire utilisée par la structure regex

    if (reti) {
        printf("nonnononon");
        return 0;  // La chaîne ne correspond pas à l'expression régulière, ce n'est pas un JSON valide
    }

    // Vérifier que les nombres et les chaînes ne sont pas entourés de guillemets
    char *pointeur = (char *)data;
    while (*pointeur) {
        if (*pointeur == '"') {
            // Si un guillemet est trouvé, vérifier le contexte
            char *suivant = pointeur + 1;
            while (*suivant && (*suivant == ' ' || *suivant == '\t' || *suivant == '\n' || *suivant == '\r')) {
                // Ignorer les espaces, tabulations et sauts de ligne
                suivant++;
            }
            if (*suivant == '-' || (*suivant >= '0' && *suivant <= '9')) {
                // Si le caractère suivant est un chiffre ou un signe moins, le guillemet est incorrect
                printf("non");
                return 0;
            }
        }
        pointeur++;
    }
    printf("pouii");
    return 1;  // La chaîne correspond à la structure JSON et respecte la condition sur les guillemets
    // //On verifie bien qu'on commence par une "{"
    // if(data[0] == '{'){
    //     printf("%s\n","on commence bien par une accollade");
    // }else{
    //     printf("%s\n","on ne commence pas par une accollade");   
    //     exit(0);
    // }

    // //nombre d'accolade ouvrante
    // int compteur1 = 0;
    // //nombre d'accolade fermante
    // int compteur2 = 0;

    // int index = 0;
    // // Parcourir chaque caractère de la chaîne data
    // while (*data != '\0') {
    //     // Vérifier si le caractère actuel est une accolade
    //     if (*data == '{') {
    //         compteur1++;
    //         while (isspace_custom(*data)) {
    //             salut++;
    //         }
    //     }
    //     if (*data == '}') {
    //         compteur2++;
    //     }

    //     // Passer au caractère suivant
    //     data++;
    //     //si on est au dernier carcactere (\0) alors on va regarder si le dernier caracter - 1 est bien une accolade fermante
    //     if(*data == '\0'){
    //         data--;
    //         if (*data == '}'){
    //             printf("%s\n", data);
    //             printf("%s\n", "on termine bien par une accollade");
    //         }else{
    //             printf("%s\n", "on ne termine pas par une accollade");
    //             exit(0);
    //         }
    //         data++;
    //     }
    //     if(index == 4){
    //         printf("%d\n", data[index]);
    //         printf("%s\n", data);
    //     }
    //     index++;
    // }
    // if(compteur1 == 1){
    //     printf("%s\n","on a bien qu'une seule accolade ouvrante");
    //     if(compteur2 == 1){
    //         printf("%s\n","on a bien qu'une seule accolade fermante");

    //     }else{
    //         printf("%s\n","on a pas qu'une seule accolade fermante");
    //         exit(0);
    //     }
    // }else{
    //     printf("%s\n","on a pas qu'une seule accolade ouvrante");
    //     exit(0);
    // }



    //printf("%d\n",data[1]);
    return 0;
}

//fonction qui permet de lire le prochain caractere en ignorant les espaces ou tabulations
int isspace_custom(char caractere) {
    return caractere == ' ' || caractere == '\t';
}

