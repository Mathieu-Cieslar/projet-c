
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

int validationJSON(char data[1024]){

    //On verifie bien qu'on commence par une "{"
    if(data[0] == '{'){
        printf("%s\n","on commence bien par une accollade");
    }else{
        printf("%s\n","on ne commence pas par une accollade");   
        exit(0);
    }

    //nombre d'accolade ouvrante
    int compteur1 = 0;
    //nombre d'accolade fermante
    int compteur2 = 0;

    int index = 0;
    // Parcourir chaque caractère de la chaîne data
    while (*data != '\0') {
        // Vérifier si le caractère actuel est une accolade
        if (*data == '{') {
            compteur1++;
            while (isspace_custom(*data)) {
                salut++;
            }
        }
        if (*data == '}') {
            compteur2++;
        }

        // Passer au caractère suivant
        data++;
        //si on est au dernier carcactere (\0) alors on va regarder si le dernier caracter - 1 est bien une accolade fermante
        if(*data == '\0'){
            data--;
            if (*data == '}'){
                printf("%s\n", data);
                printf("%s\n", "on termine bien par une accollade");
            }else{
                printf("%s\n", "on ne termine pas par une accollade");
                exit(0);
            }
            data++;
        }
        if(index == 4){
            printf("%d\n", data[index]);
            printf("%s\n", data);
        }
        index++;
    }
    if(compteur1 == 1){
        printf("%s\n","on a bien qu'une seule accolade ouvrante");
        if(compteur2 == 1){
            printf("%s\n","on a bien qu'une seule accolade fermante");

        }else{
            printf("%s\n","on a pas qu'une seule accolade fermante");
            exit(0);
        }
    }else{
        printf("%s\n","on a pas qu'une seule accolade ouvrante");
        exit(0);
    }



    //printf("%d\n",data[1]);
    return 0;
}

//fonction qui permet de lire le prochain caractere en ignorant les espaces ou tabulations
int isspace_custom(char caractere) {
    return caractere == ' ' || caractere == '\t';
}

