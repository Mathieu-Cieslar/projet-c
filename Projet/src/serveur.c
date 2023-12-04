/*
 * SPDX-FileCopyrightText: 2021 John Samuel
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

#include "serveur.h"
#include "validation.h"
#include "json.h"
int socketfd;

int visualize_plot()
{
  const char *browser = "firefox";

  char command[256];
  snprintf(command, sizeof(command), "%s %s", browser, svg_file_path);

  int result = system(command);

  if (result == 0)
  {
    printf("SVG file opened in %s.\n", browser);
  }
  else
  {
    printf("Failed to open the SVG file.\n");
  }

  return 0;
}

/* renvoyer un message (*data) au client (client_socket_fd)
 */
int renvoie_message(int client_socket_fd, char *data)
{

  if (validationJSON(data) == 0){
    printf("%s\n","JSON envoyé non valide");
    return 0;
  };
  if (ValidationAvantEnvoieServeur(data) == 0){
    printf("%s\n","JSON envoyé non valide");
    return 0;
  };

  int data_size = write(client_socket_fd, (void *)data, strlen(data));

  if (data_size < 0)
  {
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

double degreesToRadians(double degrees)
{
  return degrees * M_PI / 180.0;
}

//fonction pour enregistrer la liste des couleurs dans un fichiers
void enregistrerCouleursDansFichier(const char *nomFichier, const char *listeCouleurs) {
    int nbCouleurs;
    const char *delim = ",";
    char *token, *copy;

    // On copie la liste de couleurs pour la modification
    copy = strdup(listeCouleurs);

    formatJsonForList(copy);

    // On verifie que la chaîne commence par un nombre et extraie le nombre de couleurs
    if (sscanf(copy, "couleurs: %d,", &nbCouleurs) != 1) {
        sscanf(copy, " %d,", &nbCouleurs);
    }

    FILE *fichier = fopen(nomFichier, "w");

    if (fichier == NULL) {
        perror("Impossible d'ouvrir le fichier");
        free(copy);
        return;
    }

    // On ecrit le nombre de couleurs enregistré dans le fichier
    fprintf(fichier, "nbCouleur enregistre %d\n", nbCouleurs);

    // On utilise strtok pour extraire les couleurs et les écrire dans le fichier
    token = strtok(copy, delim);
    // On avance pour ignorer le nombre
    token = strtok(NULL, delim);
    while (token != NULL && nbCouleurs > 0) {
        fprintf(fichier, "%s\n", token);
        token = strtok(NULL, delim);
        nbCouleurs--;
    }

    fclose(fichier);
    free(copy);
}

//fonction qui permet d enregistrer une liste de balise dans un fichier 
void enregistrerBalisesDansFichier(const char *nomFichier, const char *listeBalises) {
    int nbBalises;
    const char *delim = ",";
    char *token, *copy;

    // On copie la liste des balises pour la modification
    copy = strdup(listeBalises);

    // on formate la list si elle provient du json
    formatJsonForList(copy);

    // VOn verifie que la chaîne commence par un nombre et extraie le nombre de couleurs
    if (sscanf(copy, "balises: %d,", &nbBalises) != 1) {
        sscanf(copy, " %d,", &nbBalises);
    }

    FILE *fichier = fopen(nomFichier, "w");

    if (fichier == NULL) {
        perror("Impossible d'ouvrir le fichier");
        free(copy);
        return;
    }

    // On ecrit le nombre de balise enregistré dans le fichier
    fprintf(fichier, "nbBalises enregistre %d\n", nbBalises);

    // On utilise strtok pour extraire les balises et les écrire dans le fichier
    token = strtok(copy, delim);
    // On avance pour ignorer le nombre
    token = strtok(NULL, delim);
    while (token != NULL && nbBalises > 0) {
        fprintf(fichier, "%s\n", token);
        token = strtok(NULL, delim);
        nbBalises--;
    }

    fclose(fichier);
    free(copy);
}



//fonction pour effectuer une opertaion en fonction du signe et de 2 nombres 
double evalOp(char *expression) {

    // On utilise strtok pour extraire le code, dans le cas d'un passage de données sans JSON
    char *codeToken = strtok(expression, ": ");
    char *operateur;

    //Si calcule est trouvé on passe au suivant (dans le cas d un message sans json)
    if (strcmp(codeToken ,"calcule") == 0) {
       operateur = strtok(NULL, " ");
    } else {
        // Si le message calcule n'est pas trouvé, l'opérateur est au début de la chaîne (cas message json)
         operateur = codeToken;
     }

    // Si operateur est NULL à ce stade, il n'y a pas d'opérateur valide
    if (operateur == NULL) {
        printf("Expression invalide\n");
        exit(1);
    }

    switch (operateur[0]) {
        case '+': {
            float nb1 = atof(strtok(NULL, " "));
            float nb2 = atof(strtok(NULL, " "));
            return nb1 + nb2;
        }
        case '-': {
            float nb1 = atof(strtok(NULL, " "));
            float nb2 = atof(strtok(NULL, " "));
            return nb1 - nb2;
        }
        case '*': {
            float nb1 = atof(strtok(NULL, " "));
            float nb2 = atof(strtok(NULL, " "));
            return nb1 * nb2;
        }
        case '/': {
            float nb1 = atof(strtok(NULL, " "));
            float nb2 = atof(strtok(NULL, " "));
            if (nb2 != 0) {
                return (double)nb1 / nb2;
            } else {
                printf("Division par 0\n");
                exit(1);
            }
        }
        default:
            printf("Opérateur non reconnu\n");
            exit(1);
    }
}


//Fonction pour receptionner un message json et effectuer le bon traitement en fonction du code
void traiterMessageJSON(int client_socket_fd,const char *jsonString) {

TableauDeChaines result = extraireCodeEtValeurs(jsonString);

char sortie[1024];
char data[1024];
 char dataToFormat[1024];
   // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

strcpy(dataToFormat, result.code);
strcat(dataToFormat, ": ");

    

if (strcmp(result.code, "message") == 0) {
  //On renvoie le message apres lu le json
  formatStringForJson(result.valeurs);
strcat(dataToFormat, result.valeurs);
strcat(dataToFormat, "\0");
strncpy(data,dataToFormat,strlen(dataToFormat)-1);

// on formate la sortie au format json
    formaterMessage(data, sortie);

      renvoie_message(client_socket_fd,sortie);
        
        //On renvoie le nom apres avoir decoder le json
    } else if (strcmp(result.code, "nom") == 0) {

      formatStringForJson(result.valeurs);
strcat(dataToFormat, result.valeurs);
strcat(dataToFormat, "\0");
strncpy(data,dataToFormat,strlen(dataToFormat)-1);

// on formate la sortie au format json
    formaterMessage(data, sortie);
     renvoie_message(client_socket_fd,sortie);

    }else if (strcmp(result.code, "calcule") == 0) {

      
      // on format les valeur recu pour pouvoir les traiter
      formatStringForJson(result.valeurs);
      //printf("data :  %s\n", result.valeurs);
      // On effecture l operation et on renvoir le resutats
      double resultats = evalOp(result.valeurs);
      char chaine[50];
      snprintf(chaine,50, "%f", resultats);

strcat(dataToFormat, chaine);
strcat(dataToFormat, "\0");
strncpy(data,dataToFormat,strlen(dataToFormat)-1);



// on formate la sortie au format json
    formaterMessage(data, sortie);

      //printf("Resultat :  %f\n", resultats);
      renvoie_message(client_socket_fd, sortie);
     
    } else if (strcmp(result.code, "couleurs") == 0) {

        //On traite la liste de couleurs sous le format json
          enregistrerCouleursDansFichier("couleur.txt",result.valeurs);

strcat(dataToFormat, "enregistré");
strcat(dataToFormat, "\0");
strncpy(data,dataToFormat,strlen(dataToFormat)-1);

// on formate la sortie au format json
    formaterMessage(data, sortie);


    renvoie_message(client_socket_fd, sortie);
      // on traite les balise sous format json

    }else if (strcmp(result.code, "balises") == 0) {
      enregistrerBalisesDansFichier("balise.txt",result.valeurs);
strcat(dataToFormat, "enregistré");
strcat(dataToFormat, "\0");
strncpy(data,dataToFormat,strlen(dataToFormat)-1);

// on formate la sortie au format json
    formaterMessage(data, sortie);
  
    renvoie_message(client_socket_fd, sortie);
    }
     else {
        printf("Choix non valide.\n");
    }



}

int plot(char *data)
{
  int i;
  char *saveptr = NULL;
  char *str = data;
  char *token = strtok_r(str, ",", &saveptr);
  const int num_colors = 10;

  double angles[num_colors];
  memset(angles, 0, sizeof(angles));

  FILE *svg_file = fopen(svg_file_path, "w");
  if (svg_file == NULL)
  {
    perror("Error opening file");
    return 1;
  }

  fprintf(svg_file, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
  fprintf(svg_file, "<svg width=\"400\" height=\"400\" xmlns=\"http://www.w3.org/2000/svg\">\n");
  fprintf(svg_file, "  <rect width=\"100%%\" height=\"100%%\" fill=\"#ffffff\" />\n");

  double center_x = 200.0;
  double center_y = 200.0;
  double radius = 150.0;

  double start_angle = -90.0;

  str = NULL;
  i = 0;
  while (1)
  {
    token = strtok_r(str, ",", &saveptr);
    if (token == NULL)
    {
      break;
    }
    str = NULL;
    angles[i] = 360.0 / num_colors;

    double end_angle = start_angle + angles[i];

    double start_angle_rad = degreesToRadians(start_angle);
    double end_angle_rad = degreesToRadians(end_angle);

    double x1 = center_x + radius * cos(start_angle_rad);
    double y1 = center_y + radius * sin(start_angle_rad);
    double x2 = center_x + radius * cos(end_angle_rad);
    double y2 = center_y + radius * sin(end_angle_rad);

    fprintf(svg_file, "  <path d=\"M%.2f,%.2f A%.2f,%.2f 0 0,1 %.2f,%.2f L%.2f,%.2f Z\" fill=\"%s\" />\n",
            x1, y1, radius, radius, x2, y2, center_x, center_y, token);

    start_angle = end_angle;
    i++;
  }

  fprintf(svg_file, "</svg>\n");

  fclose(svg_file);

  visualize_plot();
  return 0;
}



/* accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie_message(int client_socket_fd, char data[1024])
{
  /*
   * extraire le code des données envoyées par le client.
   * Les données envoyées par le client peuvent commencer par le mot "message :" ou un autre mot.
   */

//On extrait le code qui sert a categoriser l'operation
  char code[10];
  sscanf(data, "%s", code);

  // On regarde si on recoit le code message 
  if (strcmp(code, "message:") == 0)
  { // On affiche le message
    printf("Message recu: %s\n", data);
    //On renvoie le message au client
    renvoie_message(client_socket_fd, data);
  }
  // on regarde si on recoit un calcul
  if (strcmp(code, "calcule:") == 0)
  { 
    printf("Calcul recu: %s\n", data);
    printf("%s",data);
    //on calcule le resultat
  double result = evalOp(data);
  printf("Resultat :  %f\n", result);
    char chaine[50];
    //on affiche le resultat du calcul
    snprintf(chaine,50, "%f", result);
    // on retourne le resultat au client
    renvoie_message(client_socket_fd, chaine);
  } // on regarde si on recoit le code nom
  if (strcmp(code, "nom:") == 0)
  {
    printf("%s\n", data);
    renvoie_message(client_socket_fd, data);
  }// on regarde si on recoit le code couleurs
  if(strcmp(code, "couleurs:") == 0)
  {
    printf("Couleur recu: %s\n", data);
    enregistrerCouleursDansFichier("couleur.txt",data);
    renvoie_message(client_socket_fd, "Couleur Sauvegardé");
  }// On regarde si on recoit le code balise
  if(strcmp(code, "balises:") == 0)
  {
    printf("Balises recu: %s\n", data);
    enregistrerBalisesDansFichier("balise.txt",data);
    renvoie_message(client_socket_fd, "Balise Sauvegardé");
  }
  //On regarde si le code est couleurs pour afficher l image
  if(strcmp(code, "couleurs:") == 1)
  {
    printf("Image recu: %s\n", data);
    plot(data);
  }// On regarde si on recoit du json et on fait le traitement adequat
  if(strchr(code, '{') != NULL){
    printf("%s \n",data);
     if (validationJSON(data) == 0){
       printf("%s\n","JSON reçu non valide");
       return 0;
     };
    traiterMessageJSON(client_socket_fd,data);
  }
  return (EXIT_SUCCESS);
}

// Fonction de gestion du signal Ctrl+C
void gestionnaire_ctrl_c(int signal)
{
  printf("\nSignal Ctrl+C capturé. Sortie du programme.\n");
  // fermer le socket
  close(socketfd);
  exit(0); // Quitter proprement le programme.
}

int main()
{
  int bind_status;

  struct sockaddr_in server_addr;

  /*
   * Creation d'une socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0)
  {
    perror("Unable to open a socket");
    return -1;
  }

  int option = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  // détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Relier l'adresse à la socket
  bind_status = bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (bind_status < 0)
  {
    perror("bind");
    return (EXIT_FAILURE);
  }

  // Enregistrez la fonction de gestion du signal Ctrl+C
  signal(SIGINT, gestionnaire_ctrl_c);

  // Écouter les messages envoyés par le client en boucle infinie
  while (1)
  {
    // Écouter les messages envoyés par le client
    listen(socketfd, 10);

    // Lire et répondre au client
    struct sockaddr_in client_addr;
    char data[1024];

    unsigned int client_addr_len = sizeof(client_addr);

    // nouvelle connection de client
    int client_socket_fd = accept(socketfd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_socket_fd < 0)
    {
      perror("accept");
      return (EXIT_FAILURE);
    }

    // la réinitialisation de l'ensemble des données
    memset(data, 0, sizeof(data));

    // lecture de données envoyées par un client
    int data_size = read(client_socket_fd, (void *)data, sizeof(data));

    if (data_size < 0)
    {
      perror("erreur lecture");
      return (EXIT_FAILURE);
    }

    recois_envoie_message(client_socket_fd, data);
  }

  return 0;
}
