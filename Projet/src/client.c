/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "client.h"
#include "bmp.h"
#include "json.h"

/*
 * Fonction d'envoi et de réception de messages
 * Il faut un argument : l'identifiant de la socket
 */

int envoie_recois_message(int socketfd)
{

  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // On demande à l'utilisateur d'entrer un message
  char message[1024];
  printf("Votre message (max 1000 caracteres): ");
  fgets(message, sizeof(message), stdin);
  strcpy(data, "message: ");
  strcat(data, message);

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("Message recu: %s\n", data);

  return 0;
}


//fonction qui permet d envoyer le message au format json au serveur
int envoie_json(int socketfd, char* code)
{
int res ;
  char data[1024];
  char dataToFormat[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

if (strcmp(code, "message") == 0) {
  // on format le message utilisateur au format json
      char message[1024];
    printf("Votre message (max 1000 caracteres): ");
    fgets(message, sizeof(message), stdin);
    strncpy(dataToFormat,message,strlen(message)-1);
    strcpy(dataToFormat, "message: ");
    strcat(dataToFormat, message);
        strncpy(data,dataToFormat,strlen(dataToFormat)-1);
        
    } else if (strcmp(code, "nom") == 0) {
      //On format le nom utilisateur au format json
       char nom[1024];
      res = gethostname(&nom[0],1024);
      strcpy(data, "nom: ");
      strcat(data, nom);
    }else if (strcmp(code, "calcule") == 0) {
// On saisit le calcul au format json
        char calcul[1024];
  printf("Entrez votre calcul sous la forme (operateur num1 num2) : ");
  fgets(calcul, sizeof(calcul), stdin);
  strcpy(dataToFormat, "calcule: ");
  strcat(dataToFormat, calcul);
  strncpy(data,dataToFormat,strlen(dataToFormat)-1);
     
    } else if (strcmp(code, "couleurs") == 0) {
      //on saisie la liste des couleurs au format json
       char listC[1024];
  printf("Entrez votre liste de couleurs sous la forme : (nbCouleur,couleur1,...) : ");
fgets(listC, sizeof(listC), stdin);
  strcpy(dataToFormat, "couleurs: ");
  strcat(dataToFormat, listC);
  strncpy(data,dataToFormat,strlen(dataToFormat)-1);
      
    }else if (strcmp(code, "balises") == 0) {
      //on saisie la liste des balise au format json
  char listB[1024];
  printf("Entrez votre liste de balises sous la forme : (nbBalise,balise1,...) : ");
fgets(listB, sizeof(listB), stdin);
  strcpy(dataToFormat, "balises: ");
  strcat(dataToFormat, listB);
  strncpy(data,dataToFormat,strlen(dataToFormat)-1);
    }
     else {
        printf("Choix non valide.\n");
    }
char sortie[200];
// on formate la sortie au format json
    formaterMessage(data, sortie);

  if (res < 0)
  {
    perror("erreur ");
    exit(EXIT_FAILURE);
  }

  int write_status = write(socketfd, sortie, strlen(sortie));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("%s\n", data);

  return 0;
}

//fonction pour envoyer le nom du client au serveur
int envoie_nom_client(int socketfd)
{
int res ;
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // On demande à l'utilisateur d'entrer un message
  char nom[1024];
 res = gethostname(&nom[0],1024);
  strcpy(data, "nom: ");
  strcat(data, nom);

  if (res < 0)
  {
    perror("erreur get host name");
    exit(EXIT_FAILURE);
  }

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("%s\n", data);

  return 0;
}

//fonction pour envoyer une operation au serveur
int envoie_operateur_numero(int socketfd)
{
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // On demande à l'utilisateur d'entrer un message
  char calcul[1024];
  printf("Entrez votre calcul sous la forme (operateur num1 num2) : ");
fgets(calcul, sizeof(calcul), stdin);
  strcpy(data, "calcule: ");
  strcat(data, calcul);


  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("%s\n", data);

  return 0;
}

//fonction pour envoyer la liste des couleurs
int envoie_list_couleurs(int socketfd)
{
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // Demandez à l'utilisateur d'entrer la liste des couleurs
  char listC[1024];
  printf("Entrez votre liste de couleurs sous la forme : (nbCouleur,couleur1,...) : ");
fgets(listC, sizeof(listC), stdin);
  strcpy(data, "couleurs: ");
  strcat(data, listC);


  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("%s\n", data);

  return 0;
}

//fonction pour envoyer la liste des balise
int envoie_list_balises(int socketfd)
{
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // On demande à l'utilisateur d'entrer une liste de balise
  char listB[1024];
  printf("Entrez votre liste de balises sous la forme : (nbBalise,balise1,...) : ");
fgets(listB, sizeof(listB), stdin);
  strcpy(data, "balises: ");
  strcat(data, listB);


  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // on réinitialise l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("%s\n", data);

  return 0;
}

void analyse(char *pathname, char *data, char* nbCouleur)
{
  // compte de couleurs
  couleur_compteur *cc = analyse_bmp_image(pathname);

  int count;
    strcpy(data, "couleurs: ");
  char temp_string[10] = "10,";
  if (strcmp(nbCouleur,"") == 0)
  {
  if (cc->size < 10)
  {
    sprintf(temp_string, "%d,", cc->size);
  }
 
  }else{
    char virgule[] = ",";
    strcpy(temp_string,nbCouleur);
     // Ajouter une virgule à la fin de la chaîne
    strcat(temp_string, virgule);
  }
   strcat(data, temp_string);

  // choisir 10 couleurs
  for (count = 1; count < atoi(nbCouleur)+1 && cc->size - count > 0; count++)
  {
    if (cc->compte_bit == BITS32)
    {
      sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc24[cc->size - count].c.rouge, cc->cc.cc32[cc->size - count].c.vert, cc->cc.cc32[cc->size - count].c.bleu);
    }
    if (cc->compte_bit == BITS24)
    {
      sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc32[cc->size - count].c.rouge, cc->cc.cc32[cc->size - count].c.vert, cc->cc.cc32[cc->size - count].c.bleu);
    }
    strcat(data, temp_string);
  }

  // enlever le dernier virgule
  data[strlen(data) - 1] = '\0';
}

int envoie_couleurs(int socketfd, char *pathname, char* nbCouleur)
{
  char data[1024];
  memset(data, 0, sizeof(data));
  analyse(pathname, data, nbCouleur);
  printf(" data %s",data);

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  return 0;
}

int main(int argc, char **argv)
{
  int socketfd;

  struct sockaddr_in server_addr;

  /*
   * Creation d'une socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0)
  {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // demande de connection au serveur
  int connect_status = connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (connect_status < 0)
  {
    perror("connection serveur");
    exit(EXIT_FAILURE);
  }

//si la commande ne coomporte pas au moins 2 arguments on affiche une erreur
  if (argc < 2 ) 
  {
    puts("Ereur dans la commande le client necessite 1 arguments et eventuellement un deuxieme argument pour l envoi en json \n");
  }

//on regarde si la commande demande le nom
  if ( strcmp( argv[1],"nom") == 0 ) 
  {
    //on regarde si le parametre json a ete active
    if ((argc==3)&& strcmp( argv[2],"json") == 0 ) 
  {
    envoie_json(socketfd,argv[1]);
  }else{
    envoie_nom_client(socketfd);
  }
  }
  else if (strcmp( argv[1], "calcule" ) ==0) 
  {  
    if ((argc==3)&& strcmp( argv[2],"json") == 0 ) 
  {
    envoie_json(socketfd,argv[1]);
  }else{
    envoie_operateur_numero(socketfd);
  }
  }
  else if ( strcmp( argv[1], "couleurs" ) ==0) 
  {
      if ((argc==3)&& strcmp( argv[2],"json") == 0 ) 
  {
    envoie_json(socketfd,argv[1]);
  }else{
    envoie_list_couleurs(socketfd);
  }
  }
  else if (strcmp( argv[1], "message")==0 )
  {
      if ((argc==3)&& strcmp( argv[2],"json") == 0 ) 
  {
    envoie_json(socketfd,argv[1]);
  }else{
    // envoyer et recevoir un message
    envoie_recois_message(socketfd);
  }
  }
    else if ( strcmp( argv[1], "balises")==0 )
  {
      if ((argc==3)&& strcmp( argv[2],"json") == 0 ) 
  {
    envoie_json(socketfd,argv[1]);
  }else{
    // envoyer et recevoir un message
    envoie_list_balises(socketfd);
  }
    
  }
  else if ( strstr(argv[1],".bmp") != NULL)
  {
    if (argc == 3)
    {
          // envoyer et recevoir les couleurs prédominantes
    // d'une image au format BMP (argv[1])
    envoie_couleurs(socketfd, argv[1],argv[2]);
    }else{
    // envoyer et recevoir les couleurs prédominantes
    // d'une image au format BMP (argv[1])
    envoie_couleurs(socketfd, argv[1],"");
    }
  
  }
else{
  puts("commande non reconnue");
}

  close(socketfd);
}
