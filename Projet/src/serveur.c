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

void enregistrerCouleursDansFichier(const char *nomFichier, const char *listeCouleurs) {
    int nbCouleurs;
    const char *delim = ",";
    char *token, *copy;

    // Copiez la liste de couleurs pour la modification
    copy = strdup(listeCouleurs);

    // Vérifiez que la chaîne commence par un nombre et extraie le nombre de couleurs
    if (sscanf(copy, "couleurs: %d,", &nbCouleurs) != 1) {
        printf("Format invalide pour la liste de couleurs.\n");
        free(copy);
        return;
    }

    FILE *fichier = fopen(nomFichier, "w");

    if (fichier == NULL) {
        perror("Impossible d'ouvrir le fichier");
        free(copy);
        return;
    }

    // Écrivez le nombre de couleurs enregistré dans le fichier
    fprintf(fichier, "nbCouleur enregistre %d\n", nbCouleurs);

    // Utilisez strtok pour extraire les couleurs et les écrire dans le fichier
    token = strtok(copy, delim);
    // Avancez pour ignorer le nombre
    token = strtok(NULL, delim);
    while (token != NULL && nbCouleurs > 0) {
        fprintf(fichier, "%s\n", token);
        token = strtok(NULL, delim);
        nbCouleurs--;
    }

    fclose(fichier);
    free(copy);
}

void enregistrerBalisesDansFichier(const char *nomFichier, const char *listeBalises) {
    int nbBalises;
    const char *delim = ",";
    char *token, *copy;

    // Copiez la liste des balises pour la modification
    copy = strdup(listeBalises);

    // Vérifiez que la chaîne commence par un nombre et extraie le nombre de couleurs
    if (sscanf(copy, "balises: %d,", &nbBalises) != 1) {
        printf("Format invalide pour la liste de balises.\n");
        free(copy);
        return;
    }

    FILE *fichier = fopen(nomFichier, "w");

    if (fichier == NULL) {
        perror("Impossible d'ouvrir le fichier");
        free(copy);
        return;
    }

    // Écrivez le nombre de balise enregistré dans le fichier
    fprintf(fichier, "nbBalises enregistre %d\n", nbBalises);

    // Utilisez strtok pour extraire les balises et les écrire dans le fichier
    token = strtok(copy, delim);
    // Avancez pour ignorer le nombre
    token = strtok(NULL, delim);
    while (token != NULL && nbBalises > 0) {
        fprintf(fichier, "%s\n", token);
        token = strtok(NULL, delim);
        nbBalises--;
    }

    fclose(fichier);
    free(copy);
}


void formatStringForJson(char *chaine) {
    // Initialiser un index pour la nouvelle chaîne
    int nouvelIndex = 0;

    // Parcourir chaque caractère de la chaîne
    for (int i = 0; i < strlen(chaine); i++) {
        // Ignorer les espaces, guillemets et virgules
        if (chaine[i] != '"' && chaine[i] != ',') {
            // Ajouter le caractère à la nouvelle chaîne
            chaine[nouvelIndex++] = chaine[i];
        }
    }

    // Ajouter le caractère nul à la fin de la nouvelle chaîne
    chaine[nouvelIndex] = '\0';
}

double evalOp(char *expression) {
    char *token = strtok(expression, " "); // get the string message
    token = strtok(NULL, " "); // get the operand
    char *operateur = token;
    printf(" op1 %d ",operateur[0]);
    switch (operateur[0]) {
        case '+':
            {
                float nb1 = atof(strtok(NULL, " "));
                float nb2 = atof(strtok(NULL, " "));
                return nb1 + nb2;
            }
        case '-':
            {
                float nb1 = atof(strtok(NULL, " "));
                float nb2 = atof(strtok(NULL, " "));
                return nb1 - nb2;
            }
        case '*':
            {
                float nb1 = atof(strtok(NULL, " "));
                float nb2 = atof(strtok(NULL, " "));
                return nb1 * nb2;
            }
        case '/':
            {
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
            printf("Expression non reconnue\n");
            exit(1);
    }
}


void traiterMessageJSON(int client_socket_fd,const char *jsonString) {
    // Recherche de la position de la première occurrence du champ "code"
    const char *codeDebut = strstr(jsonString, "\"code\"");

    // Vérifie si le JSON contient le champ "code"
    if (codeDebut == NULL) {
        fprintf(stderr, "Le champ 'code' est introuvable dans le JSON.\n");
        return;
    }

    // Avance le pointeur au début de la valeur associée au champ "code"
    codeDebut = strchr(codeDebut, ':');
    if (codeDebut == NULL) {
        fprintf(stderr, "Format JSON invalide.\n");
        return;
    }
    codeDebut++; // Avance après le caractère ':'

    // Ignore les espaces potentiels après le caractère ':' et avant le début de la chaîne de caractères
    while (*codeDebut == ' ' || *codeDebut == '\t' || *codeDebut == '\n' || *codeDebut == '\r') {
        codeDebut++;
    }

    // Vérifie si la valeur associée au champ "code" commence par un guillemet
    if (*codeDebut != '\"') {
        fprintf(stderr, "Format JSON invalide pour la valeur associée au champ 'code'.\n");
        return;
    }

    // Avance le pointeur au début du code
    codeDebut++;

    // Recherche de la position de la première occurrence du caractère '"'
    const char *codeFin = strchr(codeDebut, '"');

    // Vérifie si le JSON contient un autre '"'
    if (codeFin == NULL) {
        fprintf(stderr, "Format JSON invalide.\n");
        return;
    }

    // Calcul de la longueur du code
    size_t longueurCode = codeFin - codeDebut;

    // Alloue de la mémoire pour le code
    char *code = malloc(longueurCode + 1);
    if (code == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        return;
    }

    // Copie le code dans une chaîne modifiable
    strncpy(code, codeDebut, longueurCode);
    code[longueurCode] = '\0'; // Ajoute le caractère de fin de chaîne

    // Affiche le code
    printf("Code: %s\n", code);

    // Recherche de la position de la première occurrence du champ "valeurs"
    const char *valeursDebut = strstr(jsonString, "\"valeurs\"");

    // Vérifie si le JSON contient le champ "valeurs"
    if (valeursDebut == NULL) {
        fprintf(stderr, "Le champ 'valeurs' est introuvable dans le JSON.\n");
        free(code);
        return;
    }

    // Avance le pointeur au début de la valeur associée au champ "valeurs"
    valeursDebut = strchr(valeursDebut, ':');
    if (valeursDebut == NULL) {
        fprintf(stderr, "Format JSON invalide.\n");
        free(code);
        return;
    }
    valeursDebut++; // Avance après le caractère ':'

    // Ignore les espaces potentiels après le caractère ':' et avant le début du tableau
    while (*valeursDebut == ' ' || *valeursDebut == '\t' || *valeursDebut == '\n' || *valeursDebut == '\r') {
        valeursDebut++;
    }

    // Vérifie si le tableau de valeurs commence bien avec un '['
    if (*valeursDebut != '[') {
        fprintf(stderr, "Format JSON invalide pour le tableau de valeurs.\n");
        free(code);
        return;
    }

    // Avance le pointeur au début du tableau
    valeursDebut++;

    // Recherche de la position de la première occurrence du caractère ']'
    const char *valeursFin = strchr(valeursDebut, ']');

    // Vérifie si le tableau de valeurs se termine bien par un ']'
    if (valeursFin == NULL) {
        fprintf(stderr, "Format JSON invalide pour le tableau de valeurs.\n");
        free(code);
        return;
    }

    // Calcul de la longueur du tableau de valeurs
    size_t longueurValeurs = valeursFin - valeursDebut;

    // Alloue de la mémoire pour le tableau de valeurs
    char *valeurs = malloc(longueurValeurs + 1);
    if (valeurs == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        free(code);
        return;
    }

    // Copie le tableau de valeurs dans une chaîne modifiable
    strncpy(valeurs, valeursDebut, longueurValeurs);
    valeurs[longueurValeurs] = '\0'; // Ajoute le caractère de fin de chaîne

    // Affiche chaque valeur individuelle du tableau
    printf("Valeurs:\n");


if (strcmp(code, "message") == 0) {
      renvoie_message(client_socket_fd,valeurs);
        
    } else if (strcmp(code, "nom") == 0) {
     renvoie_message(client_socket_fd,valeurs);
    }else if (strcmp(code, "calcule") == 0) {
      formatStringForJson(valeurs);
      printf("%s",valeurs);
        double result = evalOp(valeurs);
  printf("Resultat :  %f\n", result);
    char chaine[50];
    snprintf(chaine,50, "%f", result);
    renvoie_message(client_socket_fd, chaine);
     
    } else if (strcmp(code, "couleur") == 0) {
      
    }else if (strcmp(code, "balise") == 0) {
  
    }
     else {
        printf("Choix non valide.\n");
    }



    // Utilisation de strtok pour extraire chaque valeur du tableau
    char *valeur = strtok(valeurs, ",");
    while (valeur != NULL) {
        // Affiche chaque valeur individuelle
        printf("  %s\n", valeur);
        // Appel suivant pour obtenir la prochaine valeur
        valeur = strtok(NULL, ",");
    }


    // Libère la mémoire allouée
    free(code);
    free(valeurs);


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

  char code[10];
  sscanf(data, "%s", code);

  // Si le message commence par le mot: 'message:'
  if (strcmp(code, "message:") == 0)
  {
    printf("Message recu: %s\n", data);
    renvoie_message(client_socket_fd, data);
  }
  if (strcmp(code, "calcule:") == 0)
  {
    printf("Calcul recu: %s\n", data);
    printf("%s",data);
  double result = evalOp(data);
  printf("Resultat :  %f\n", result);
    char chaine[50];
    snprintf(chaine,50, "%f", result);
    renvoie_message(client_socket_fd, chaine);
  }
  if (strcmp(code, "nom:") == 0)
  {
    printf("%s\n", data);
    renvoie_message(client_socket_fd, data);
  }
  if(strcmp(code, "couleurs:") == 0)
  {
    printf("Couleur recu: %s\n", data);
    enregistrerCouleursDansFichier("couleur.txt",data);
    renvoie_message(client_socket_fd, "Couleur Sauvegardé");
  }
  if(strcmp(code, "balises:") == 0)
  {
    printf("Balises recu: %s\n", data);
    enregistrerBalisesDansFichier("balise.txt",data);
    renvoie_message(client_socket_fd, "Balise Sauvegardé");
  }
  if(strcmp(code, "couleurs:") == 0)
  {
    printf("Image recu: %s\n", data);
    plot(data);
  }if(strchr(code, '{') != NULL){
    printf("%s \n",data);
    traiterMessageJSON(client_socket_fd,data);
  }
  else{
    printf("Messge inconnu: %s, data :  %s\n", code,data);
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
