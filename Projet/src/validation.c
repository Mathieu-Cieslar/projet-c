
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
#include "json.h"

// Appel des fonctions pour pouvoir les utilisées dans validation JSON
int validationCode(char data[1024]);

int ValidationCalcule(char data[1024]);

int ValidationBalisesCouleurs(char data[1024]);

int validationJSON(char data[1024])
{

    if (!validationCode(data))
    {
        return 0;
    }

    regex_t regex;
    int reti;

    // Expression régulière pour vérifier la structure JSON
    const char *pattern = "\\{\\s*\"code\"\\s*:\\s*\"[^\"]+\"\\s*,\\s*\"valeurs\"\\s*:\\s*\\[.*\\]\\s*\\}";

    // Compiler l'expression régulière
    reti = regcomp(&regex, pattern, REG_EXTENDED);
    if (reti)
    {
        fprintf(stderr, "Erreur lors de la compilation de l'expression régulière\n");
        return 0; // Retourner 0 en cas d'erreur
    }

    // Exécuter la correspondance
    reti = regexec(&regex, data, 0, NULL, 0);
    regfree(&regex); // Libérer la mémoire utilisée par la structure regex

    if (reti)
    {
        return 0; // La chaîne ne correspond pas à l'expression régulière, ce n'est pas un JSON valide
    }

    // Recherche du début du tableau de valeurs
    char *start = strstr(data, "\"valeurs\" : [");
    if (start == NULL)
    {
        printf("Erreur : Impossible de trouver le tableau de valeurs.\n");
        return 0;
    }

    // Déplacement au début des valeurs
    start += strlen("\"valeurs\" : [");

    // Recherche de la fin du tableau de valeurs
    char *end = strchr(start, ']');
    if (end == NULL)
    {
        printf("Erreur : Impossible de trouver la fin du tableau de valeurs.\n");
        return 0;
    }

    // Copie des valeurs dans une nouvelle chaîne de caractères
    char *values = (char *)malloc(end - start + 1);
    strncpy(values, start, end - start);

    values[end - start] = '\0';

    // Supprime les espaces en début de chaîne
    while (values[0] == ' ')
    {
        values++;
    }

    // Supprime les espaces en fin de chaîne
    while (values[strlen(values) - 1] == ' ')
    {
        values[strlen(values) - 1] = '\0';
    }

    // On transforme la chaîne en tableau de chaînes, on compte le nombre de valeurs et on enlève les espaces
    // On ne prend en compte que les valeurs qui ne sont pas entre guillemets
    int nbValeurs = 0;
    for (int i = 0; i < strlen(values); i++)
    {
        if (values[i] == ',')
        {
            nbValeurs++;
        }
    }
    nbValeurs++;

    char **valeurs = (char **)malloc(nbValeurs * sizeof(char *));
    char *valeur = strtok(values, ",");
    int i = 0;
    while (valeur != NULL)
    {
        valeurs[i] = valeur;
        valeur = strtok(NULL, ",");
        i++;
    }

    for (int i = 0; i < nbValeurs; i++)
    {
        // supprime les espaces en début de chaîne
        while (valeurs[i][0] == ' ')
        {
            valeurs[i]++;
        }

        // supprime les espaces en fin de chaîne
        while (valeurs[i][strlen(valeurs[i]) - 1] == ' ')
        {
            valeurs[i][strlen(valeurs[i]) - 1] = '\0';
        }
    }

    // Vérifier que les valeurs sont des nombres avec un regex
    // On ne vérifie que les valeurs qui n'ont pas de guillemets
    const char *pattern2 = "^-?[0-9]+(\\.[0-9]+)?$";
    const char *pattern3 = "^\"-?[0-9]+(\\.[0-9]+)?\"$";
    if (reti)
    {
        fprintf(stderr, "Erreur lors de la compilation de l'expression régulière\n");
        return 0; // Retourner 0 en cas d'erreur
    }

    for (i = 0; i < nbValeurs; i++)
    {
        if (valeurs[i][0] != '"')
        {
            reti = regcomp(&regex, pattern2, REG_EXTENDED);
            reti = regexec(&regex, valeurs[i], 0, NULL, 0);
            if (reti)
            {
                return 0; // La chaîne ne correspond pas à l'expression régulière, ce n'est pas un JSON valide
            }
        }
        if (valeurs[i][0] == '"')
        {
            reti = regcomp(&regex, pattern3, REG_EXTENDED);
            reti = regexec(&regex, valeurs[i], 0, NULL, 0);
            if (!reti)
            {
                return 0; // La chaîne ne correspond pas à l'expression régulière, ce n'est pas un JSON valide
            }
        }
    }

    return 1; // La chaîne correspond à la structure JSON et respecte la condition sur les guillemets
}

// Fonction qui permet de valider le code du json
int validationCode(char data[1024])
{
    regex_t regex;

    int resultCode;
    const char *pattern = "(\"code\"\\s*:\\s*\"(calcule|nom|balises|couleurs|message)\")";
    resultCode = regcomp(&regex, pattern, REG_EXTENDED);
    resultCode = regexec(&regex, data, 0, NULL, 0);
    if (resultCode)
    {
        return 0; // La chaîne ne correspond pas à l'expression régulière, ce n'est pas un JSON valide
    }
    else
    {
        return 1;
    }
}

// Fonction pour valider les chaines à l'interieur du JSON
int validationValeursUniqueSTR(char data[1024])
{
    // ce regex va verifier si on a bien qu'un seul element en chaine de caractere dans la liste pour "valeurs" dans Message
    regex_t regex;

    int resultCode;
    const char *pattern = "\"valeurs\"\\s*:\\s*\\[\\s*\"([^\"]*)\"\\s*\\]";
    resultCode = regcomp(&regex, pattern, REG_EXTENDED);
    resultCode = regexec(&regex, data, 0, NULL, 0);
    if (resultCode)
    {
        printf("La valeur dans \"valeurs\" n'est pas bonne\n");
        return 0; // La chaîne ne correspond pas à l'expression régulière, ce n'est pas un JSON valide
    }
    else
    {
        return 1;
    }
}

// Fonction pour valider les nombres à l'interieur du JSON
int validationValeursUniqueINT(char data[1024])
{
    // ce regex va verifier si on a bien qu'un seul element en chaine de caractere dans la liste pour "valeurs" dans Message
    regex_t regex;

    int resultCode;
    const char *pattern = "\"valeurs\"\\s*:\\s*\\[\\s*-?[0-9]+(\\.[0-9]+)?\\s*\\]";
    resultCode = regcomp(&regex, pattern, REG_EXTENDED);
    resultCode = regexec(&regex, data, 0, NULL, 0);
    if (resultCode)
    {
        printf("La valeur dans \"valeurs\" n'est pas bonne\n");
        return 0; // La chaîne ne correspond pas à l'expression régulière, ce n'est pas un JSON valide
    }
    else
    {
        return 1;
    }
}

// Fonction pour verifier la commande calcule
int ValidationCalcule(char data[1024])
{
    TableauDeChaines result = extraireCodeEtValeurs(data);

    // on enleve les espaces dans ma chaine de caractere
    int len = strlen(result.valeurs);
    int i, j = 0;

    for (i = 0; i < len; i++)
    {
        if (result.valeurs[i] != ' ')
        {
            result.valeurs[j++] = result.valeurs[i];
        }
    }

    // Ajout du caractère nul à la fin de la nouvelle chaîne
    result.valeurs[j] = '\0';

    const char *delimiters = ",";
    char *token;

    // Utilisation de strtok pour extraire des tokens
    token = strtok(result.valeurs, delimiters);

    int index = 0;
    // Parcourir les tokens
    while (token != NULL)
    {

        if (index == 0)
        {
            regex_t regex;
            int reti;
            const char *pattern = "[minimum|maximum|ecart-type|moyenne|+|-|/|*]"; // Expression régulière pour +, -, *, /

            // Compilation de l'expression régulière
            reti = regcomp(&regex, pattern, REG_EXTENDED);
            if (reti)
            {
                fprintf(stderr, "Impossible de compiler l'expression régulière\n");
                return 0;
            }
            reti = regexec(&regex, token, 0, NULL, 0);
            if (reti)
            {
                printf("La chaîne \"%s\" ne correspond pas à l'expression régulière\n", token);
                return 0;
            }
        }
        else
        {
            regex_t regex;
            int reti;
            const char *pattern1 = "^-?[0-9]+(\\.[0-9]+)?$"; // Expression régulière pour +, -, *, /

            // Compilation de l'expression régulière
            reti = regcomp(&regex, pattern1, REG_EXTENDED);
            if (reti)
            {
                fprintf(stderr, "Impossible de compiler l'expression régulière\n");
                return 0;
            }
            reti = regexec(&regex, token, 0, NULL, 0);
            if (reti)
            {
                printf("La chaîne \"%s\" ne correspond pas à l'expression régulière\n", token);
                return 0;
            }
        }
        // Afficher ou traiter chaque token

        // Appel suivant pour obtenir le prochain token
        token = strtok(NULL, delimiters);
        index++;
    }

    return 1;
}

// Fonction qui verifie la commande balise et couleurs
int ValidationBalisesCouleurs(char data[1024])
{
    TableauDeChaines result = extraireCodeEtValeurs(data);

    // on enleve les espaces dans ma chaine de caractere
    int len = strlen(result.valeurs);
    int i, j = 0;

    for (i = 0; i < len; i++)
    {
        if (result.valeurs[i] != ' ')
        {
            result.valeurs[j++] = result.valeurs[i];
        }
    }

    // Ajout du caractère nul à la fin de la nouvelle chaîne
    result.valeurs[j] = '\0';

    const char *delimiters = ",";
    char *token;

    // Utilisation de strtok pour extraire des tokens
    token = strtok(result.valeurs, delimiters);

    int index = 0;
    int nb = 0;
    // Parcourir les tokens
    while (token != NULL)
    {

        if (index == 0)
        {
            nb = atoi(token);
            regex_t regex;
            int reti;
            const char *pattern = "^-?[0-9]+(\\.[0-9]+)?$"; // Expression régulière pour +, -, *, /

            // Compilation de l'expression régulière
            reti = regcomp(&regex, pattern, REG_EXTENDED);
            if (reti)
            {
                fprintf(stderr, "Impossible de compiler l'expression régulière\n");
                return 0;
            }
            reti = regexec(&regex, token, 0, NULL, 0);
            if (reti)
            {
                printf("La chaîne \"%s\" ne correspond pas à l'expression régulière\n", token);
                return 0;
            }
        }
        else
        {
            regex_t regex;
            int reti;
            const char *pattern1 = "^\"(\\.|[^\"\\])*\"$"; // Expression régulière pour +, -, *, /

            // Compilation de l'expression régulière
            reti = regcomp(&regex, pattern1, REG_EXTENDED);
            if (reti)
            {
                fprintf(stderr, "Impossible de compiler l'expression régulière\n");
                return 0;
            }
            reti = regexec(&regex, token, 0, NULL, 0);
            if (reti)
            {
                printf("La chaîne \"%s\" ne correspond pas à l'expression régulière\n", token);
                return 0;
            }
        }

        // Appel suivant pour obtenir le prochain token
        token = strtok(NULL, delimiters);
        index++;
    }
    index--;
    // On va verifier si on a le bon nombre d'elements avec notre premier parametre
    if (nb == index)
    {
        return 1;
    }
    else
    {
        printf("Le nb de parametre n'a pas été respecté\n");
        return 0;
    }
}

// Fonction qui va appeler les fonctions de verification selon le code avant l'envoie de la part du client
int ValidationAvantEnvoieClient(char data[1024])
{

    TableauDeChaines result = extraireCodeEtValeurs(data);
    if (strcmp(result.code, "nom") == 0)
    {
        if (validationValeursUniqueSTR(data))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (strcmp(result.code, "calcule") == 0)
    {
        if (ValidationCalcule(data))
        {
            return 1;
        }
        else
        {
            return 0;
        }
        return 1;
    }
    else if (strcmp(result.code, "couleurs") == 0)
    {
        if (ValidationBalisesCouleurs(data))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (strcmp(result.code, "balises") == 0)
    {
        if (ValidationBalisesCouleurs(data))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (strcmp(result.code, "message") == 0)
    {
        if (validationValeursUniqueSTR(data))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        printf("commande non reconnue\n");
        return 0;
    }
    printf("%s\n", result.code);
    return 1;
}

// Fonction qui va appeler les fonctions de verification selon le code avant l'envoie de la part du serveur
int ValidationAvantEnvoieServeur(char data[1024])
{

    TableauDeChaines result = extraireCodeEtValeurs(data);
    if (strcmp(result.code, "nom") == 0)
    {
        if (validationValeursUniqueSTR(data))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (strcmp(result.code, "calcule") == 0)
    {
        if (validationValeursUniqueINT(data))
        {
            return 1;
        }
        else
        {
            return 0;
        }
        return 1;
    }
    else if (strcmp(result.code, "couleurs") == 0)
    {
        if (validationValeursUniqueSTR(data))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (strcmp(result.code, "balises") == 0)
    {
        if (validationValeursUniqueSTR(data))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (strcmp(result.code, "message") == 0)
    {
        if (validationValeursUniqueSTR(data))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        printf("commande non reconnue\n");
        return 0;
    }
    printf("%s\n", result.code);
    return 1;
}