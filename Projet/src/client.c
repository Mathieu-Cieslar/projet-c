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

/*
 * Fonction d'envoi et de réception de messages
 * Il faut un argument : l'identifiant de la socket
 */

int envoie_recois_message(int socketfd)
{

  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // Demandez à l'utilisateur d'entrer un message
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

void formaterMessage(const char *entree, char *sortie) {
    const char *delim = ": ";
    char *token, *copy;
    char code[50];
    char valeurs[100];

    // Copiez l'entrée pour la modification
    copy = strdup(entree);

    // Utilisez strtok pour extraire le code
    token = strtok(copy, delim);
    if (token == NULL) {
        strcpy(sortie, "Format invalide pour l'entrée.");
        free(copy);
        return;
    }
    strcpy(code, token);

    // Utilisez strtok pour extraire les valeurs
    token = strtok(NULL, delim);
    if (token == NULL) {
        strcpy(sortie, "Format invalide pour l'entrée.");
        free(copy);
        return;
    }
    strcpy(valeurs, token);

    // Formate la sortie
    snprintf(sortie, 200, "{\n\t\"code\" : \"%s\",\n\t\"valeurs\" : [ \"%s\" ]\n}", code, valeurs);

    free(copy);
}

int envoie_json(int socketfd, char* code)
{
int res ;
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

if (strcmp(code, "message") == 0) {
      char message[1024];
    printf("Votre message (max 1000 caracteres): ");
    fgets(message, sizeof(message), stdin);
    strcpy(data, "message: ");
    strcat(data, message);
        
    } else if (strcmp(code, "nom") == 0) {
       char nom[1024];
      res = gethostname(&nom[0],1024);
      strcpy(data, "nom: ");
      strcat(data, nom);
    }else if (strcmp(code, "calcule") == 0) {
        char calcul[1024];
  printf("Entrez votre calcul sous la forme (operateur num1 num2) : ");
fgets(calcul, sizeof(calcul), stdin);
  strcpy(data, "calcule: ");
  strcat(data, calcul);
     
    } else if (strcmp(code, "couleur") == 0) {
       char listC[1024];
  printf("Entrez votre liste de couleurs sous la forme : (nbCouleur,couleur1,...) : ");
fgets(listC, sizeof(listC), stdin);
  strcpy(data, "couleurs: ");
  strcat(data, listC);
      
    }else if (strcmp(code, "balise") == 0) {
  char listB[1024];
  printf("Entrez votre liste de balises sous la forme : (nbBalise,balise1,...) : ");
fgets(listB, sizeof(listB), stdin);
  strcpy(data, "balises: ");
  strcat(data, listB);
    }
     else {
        printf("Choix non valide.\n");
    }




char sortie[200];

    formaterMessage(data, sortie);

  if (res < 0)
  {
    perror("erreur get host name");
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


int envoie_nom_client(int socketfd)
{
int res ;
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // Demandez à l'utilisateur d'entrer un message
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


int envoie_operateur_numero(int socketfd)
{
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // Demandez à l'utilisateur d'entrer un message
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

int envoie_list_couleurs(int socketfd)
{
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // Demandez à l'utilisateur d'entrer un message
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

int envoie_list_balises(int socketfd)
{
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // Demandez à l'utilisateur d'entrer un message
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

void analyse(char *pathname, char *data)
{
  // compte de couleurs
  couleur_compteur *cc = analyse_bmp_image(pathname);

  int count;
  strcpy(data, "couleurs: ");
  char temp_string[10] = "10,";
  if (cc->size < 10)
  {
    sprintf(temp_string, "%d,", cc->size);
  }
  strcat(data, temp_string);

  // choisir 10 couleurs
  for (count = 1; count < 11 && cc->size - count > 0; count++)
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

int envoie_couleurs(int socketfd, char *pathname)
{
  char data[1024];
  memset(data, 0, sizeof(data));
  analyse(pathname, data);
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

  /*if (argc < 2)
  {
    printf("usage: ./client chemin_bmp_image\n");
    return (EXIT_FAILURE);
  }*/

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

  if (argc < 2 ) 
  {
    puts("Ereur dans la commande le client necessite 1 arguments et eventuellement un deuxieme argument pour l envoi en json \n");
  }

  if ( strcmp( argv[1],"nom") == 0 ) 
  {
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
    // envoyer et recevoir les couleurs prédominantes
    // d'une image au format BMP (argv[1])
    envoie_couleurs(socketfd, argv[1]);
  }
    else if (strcmp( argv[1], "liste")==0)
  {
    // envoyer et recevoir les couleurs prédominantes
    // d'une image au format BMP (argv[1])
    envoie_json(socketfd,argv[1]);
    //envoie_couleurs(socketfd, argv[1]);
  }
else{
  puts("commande non reconnue");
}

  close(socketfd);
}
