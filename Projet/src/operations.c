
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

// Fonction pour calculer une moyenne
float calculerMoyenne(char *tableau[], int taille)
{
    float somme = 0.0;

    for (int i = 0; i < taille; i++)
    {

        somme += atof(tableau[i]);
    }
    // Retourne la moyenne
    return taille > 0 ? somme / taille : 0.0;
}

// Fonction pour trouver le minimum
float trouverMinimum(char *tableau[], int taille)
{
    float minimum = DBL_MAX;

    for (int i = 0; i < taille; i++)
    {
        float valeur = atof(tableau[i]);
        if (valeur < minimum)
        {
            minimum = valeur;
        }
    }

    return minimum;
}

// Fonction pour trouver le maximum
float trouverMaximum(char *tableau[], int taille)
{
    float maximum = -DBL_MAX;

    for (int i = 0; i < taille; i++)
    {
        float valeur = atof(tableau[i]);
        if (valeur > maximum)
        {
            maximum = valeur;
        }
    }

    return maximum;
}

// Fonction pour calculer l ecart type
float calculerEcartType(char *tableau[], int taille)
{
    float somme = 0.0;
    float sommeCarres = 0.0;

    for (int i = 0; i < taille; i++)
    {
        float valeur = atof(tableau[i]);
        somme += valeur;
        sommeCarres += valeur * valeur;
    }

    float moyenne = somme / taille;
    float variance = (sommeCarres / taille) - (moyenne * moyenne);

    return sqrt(variance);
}