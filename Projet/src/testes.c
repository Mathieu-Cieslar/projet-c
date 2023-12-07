
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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


#include "operations.h"
#include "validation.h"
#include "json.h"
#define EPSILON 1e-6  

int compareFloats(float a, float b) {
    return fabs(a - b) < EPSILON;
}

 int testes_unitaire(){
    char *tabMoyenne[] = {"1.0", "2.0", "3.0"};
    if(calculerMoyenne(tabMoyenne,3)==2.00){
        printf("%s\n","REUSSITE testes unitaire operation Moyenne");
    }else{
        printf("%s\n","ECHEC testes unitaire operation Moyenne");
        return 0;
    }
    char *tabMin[] = {"6.0", "2.0", "7.0"};
    if(trouverMinimum(tabMin,3)==2.0){
        printf("%s\n","REUSSITE testes unitaire operation Minimum");
    }else{
        printf("%s\n","ECHEC testes unitaire operation Minimum");
        return 0;
    }
    char *tabMax[] = {"5.0", "20.0", "3.0"};
    if(trouverMaximum(tabMax,3)==20.00){
        printf("%s\n","REUSSITE testes unitaire operation Maximum");
    }else{
        printf("%s\n","ECHEC testes unitaire operation Maximum");
        return 0;
    }
    char *tabEcartType[] = {"2.0", "4.0", "6.0","8.0","10.0"};
    if(compareFloats(calculerEcartType(tabEcartType,5), 2.828427)){
        printf("%s\n","REUSSITE testes unitaire operation EcartType");
    }else{
        printf("%s\n","ECHEC testes unitaire operation EcartType");
        return 0;
    }

    char data1[1024] = "{\"code\" : \"calcule\",\"valeurs\" : [ 10.00000 ]}";
    if(validationJSON(data1)){
        printf("%s\n","REUSSITE testes unitaire ValidationJSON");
    }else{
        printf("%s\n","ECHEC testes unitaire ValidationJSON");
        return 0;
    }
    char data2[1024] = "{\"azeazeaze\" : \"calcule\",\"valeurs\" : }";
    if(validationJSON(data2)){
        printf("%s\n","ECHEC testes unitaire ValidationJSON");
        return 0;
    }else{
        printf("%s\n","REUSSITE testes unitaire ValidationJSON");
    }
    char data3[1024] = "{\"code\" : \"calcule\",\"valeurs\" : [ \"moyenne\", 4, 5, 6 ]}";
    if(validationJSON(data3)){
        printf("%s\n","REUSSITE testes unitaire ValidationJSON");
    }else{
        printf("%s\n","ECHEC testes unitaire ValidationJSON");
        return 0;
    }
    char data4[1024] = "{\"code\" : \"calcule\",\"valeurs\" : [ \"moyenne\", \"4\", 5, 6 ]}";
    if(validationJSON(data4)){
        printf("%s\n","ECHEC testes unitaire ValidationJSON");
        return 0;
    }else{
        printf("%s\n","REUSSITE testes unitaire ValidationJSON");
    }
     
    char entree1[1024];
    char sortie1[1024];
    strcpy(entree1,"calcule: + 5 5");
    formaterMessage(entree1,sortie1);
    if(strcmp(sortie1,"{\n\t\"code\" : \"calcule\",\n\t\"valeurs\" : [ \"+\", 5, 5 ]\n}")==0){
        printf("%s\n","Encodage JSON fonctionnel");
    }else{
        printf("%s\n","Encodage JSON non fonctionnel");
        return 0;
    }
    return 1;
 }

/* 
 * L'objectif principal de ce code est d'effectuer des tests unitaires et
 *  fonctionnels sur les différents composants créés dans ce projet.
 */
