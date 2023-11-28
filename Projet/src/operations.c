
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* 
 * Code pour traiter différents types d'opérations mathématiques
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <float.h>
#include <math.h>
#include "operations.h"

double calculerMoyenne(char *tableau[], int taille) {
    double somme = 0.0;

    for (int i = 0; i < taille; i++) {
    
        somme += atof(tableau[i]);
    }
    // Retourne la moyenne
    return taille > 0 ? somme / taille : 0.0;
}

double trouverMinimum(char *tableau[], int taille) {
    double minimum = DBL_MAX;

    for (int i = 0; i < taille; i++) {
        double valeur = atof(tableau[i]);
        if (valeur < minimum) {
            minimum = valeur;
        }
    }

    return minimum;
}

double trouverMaximum(char *tableau[], int taille) {
    double maximum = -DBL_MAX;

    for (int i = 0; i < taille; i++) {
        double valeur = atof(tableau[i]);
        if (valeur > maximum) {
            maximum = valeur;
        }
    }

    return maximum;
}

double calculerEcartType(char *tableau[], int taille) {
    double somme = 0.0;
    double sommeCarres = 0.0;

    for (int i = 0; i < taille; i++) {
        double valeur = atof(tableau[i]);
        somme += valeur;
        sommeCarres += valeur * valeur;
    }

    double moyenne = somme / taille;
    double variance = (sommeCarres / taille) - (moyenne * moyenne);

    return sqrt(variance);
}