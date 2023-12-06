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
#include <sys/select.h>

#define MAX_CLIENTS 10

#include "serveur.h"
#include "json.h"
#include "operations.h"
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

// fonction pour enregistrer la liste des couleurs dans un fichiers
void enregistrerCouleursDansFichier(const char *nomFichier, const char *listeCouleurs)
{
  int nbCouleurs;
  const char *delim = ",";
  char *token, *copy;

  // On copie la liste de couleurs pour la modification
  copy = strdup(listeCouleurs);

  formatJsonForList(copy);

  // On verifie que la chaîne commence par un nombre et extraie le nombre de couleurs
  if (sscanf(copy, "couleurs: %d,", &nbCouleurs) != 1)
  {
    sscanf(copy, " %d,", &nbCouleurs);
  }

  // On ouvre le fichier pour écriture
  FILE *fichier = fopen(nomFichier, "w");

  if (fichier == NULL)
  {
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
  while (token != NULL && nbCouleurs > 0)
  {
    fprintf(fichier, "%s\n", token);
    token = strtok(NULL, delim);
    nbCouleurs--;
  }

  // On ferme le fichier et on libère la memoire
  fclose(fichier);
  free(copy);
}

// fonction qui permet d enregistrer une liste de balise dans un fichier
void enregistrerBalisesDansFichier(const char *nomFichier, const char *listeBalises)
{
  int nbBalises;
  const char *delim = ",";
  char *token, *copy;

  // On copie la liste des balises pour la modification
  copy = strdup(listeBalises);

  // on formate la list si elle provient du json
  formatJsonForList(copy);

  // VOn verifie que la chaîne commence par un nombre et extraie le nombre de couleurs
  if (sscanf(copy, "balises: %d,", &nbBalises) != 1)
  {
    sscanf(copy, " %d,", &nbBalises);
  }

  // On ouvre le fichier pour écriture
  FILE *fichier = fopen(nomFichier, "w");

  if (fichier == NULL)
  {
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
  while (token != NULL && nbBalises > 0)
  {
    fprintf(fichier, "%s\n", token);
    token = strtok(NULL, delim);
    nbBalises--;
  }

  // On ferme le fichier et on libere la memoire
  fclose(fichier);
  free(copy);
}

// fonction pour effectuer une opertaion avec un operateur et 2 nombre ou une fonction avec une suite de nombre
double evalOp(char *expression)
{

  // On utilise strtok pour extraire le code, dans le cas d'un passage de données sans JSON
  char *codeToken = strtok(expression, ": ");
  char *operateur;

  // Si calcule est trouvé on passe au suivant (dans le cas d un message sans json)
  if (strcmp(codeToken, "calcule") == 0)
  {
    operateur = strtok(NULL, " ");
  }
  else
  {
    // Si le message calcule n'est pas trouvé, l'opérateur est au début de la chaîne (cas message json)
    operateur = codeToken;
  }

  // Si operateur est NULL à ce stade, il n'y a pas d'opérateur valide
  if (operateur == NULL)
  {
    printf("Expression invalide\n");
    exit(1);
  }
  // suivant l'operateur on effectue la bonne opération
  switch (operateur[0])
  {
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
    if (nb2 != 0)
    {
      return (double)nb1 / nb2;
    }
    else
    {
      printf("Division par 0\n");
      exit(1);
    }
  }
    // on entre dans le cas ou une fonction a été passé et pas un operateur
  default:
  {
    char *token = NULL;
    char *tokens[100]; // Tableau de pointeurs vers des chaînes
    int count = 0;
    // On extrait toutes les valeurs
    while (1)
    {
      token = strtok(NULL, " ");
      if (token == NULL)
        break;

      tokens[count] = strdup(token); // Stocke la copie de la chaîne dans le tableau
      count++;
    }
    if (strcmp(operateur, "moyenne") == 0)
    {
      float moyenne = calculerMoyenne(tokens, count);
      return moyenne;
    }
    else if (strcmp(operateur, "minimum") == 0)
    {
      float min = trouverMinimum(tokens, count);
      return min;
    }
    else if (strcmp(operateur, "maximum") == 0)
    {
      float max = trouverMaximum(tokens, count);
      return max;
    }
    else if (strcmp(operateur, "ecart-type") == 0)
    {
      float ecart = calculerEcartType(tokens, count);
      return ecart;
    }
    else
    {
      // aucune fonction reconnue on libère la memoire et on met un message d'erreur
      for (int i = 0; i < count; i++)
      {
        free(tokens[i]);
      }
      printf("data %s\n", strtok(NULL, " "));
      printf("Opérateur non reconnu\n");
      exit(1);
    }
  }
  }
}

// Fonction pour receptionner un message json et effectuer le bon traitement en fonction du code
void traiterMessageJSON(int client_socket_fd, const char *jsonString)
{

  // on extrait le code et les valeurs du json dans une strucutre
  TableauDeChaines result = extraireCodeEtValeurs(jsonString);

  char sortie[200];
  char data[1024];
  char dataToFormat[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // On récupère le code
  strcpy(dataToFormat, result.code);
  strcat(dataToFormat, ": ");

  // Si le code est message on lance le traitement adequat
  if (strcmp(result.code, "message") == 0)
  {

    // On format la valeur pour pouvoir la transformer en JSON
    formatStringForJson(result.valeurs);
    strcat(dataToFormat, result.valeurs);
    strcat(dataToFormat, "\0");
    strncpy(data, dataToFormat, strlen(dataToFormat) - 1);

    // on formate la sortie au format json
    formaterMessage(data, sortie);
    // On renvoie le nom apres avoir decoder le json
    renvoie_message(client_socket_fd, sortie);
  }
  else if (strcmp(result.code, "nom") == 0)
  {
    // On formate la valeur pour pouvoir la transformer en JSON
    formatStringForJson(result.valeurs);
    strcat(dataToFormat, result.valeurs);
    strcat(dataToFormat, "\0");
    strncpy(data, dataToFormat, strlen(dataToFormat) - 1);

    // On formate la sortie au format json
    formaterMessage(data, sortie);
    // On renvoie le json au client
    renvoie_message(client_socket_fd, sortie);
  }
  else if (strcmp(result.code, "calcule") == 0)
  {

    // on format les valeur recu pour pouvoir les traiter
    formatStringForJson(result.valeurs);

    // On effecture l operation et on renvoie le resutats
    double resultats = evalOp(result.valeurs);

    char chaine[50];
    snprintf(chaine, 50, "%f", resultats);

    strcat(dataToFormat, chaine);
    strcat(dataToFormat, "\0");
    strncpy(data, dataToFormat, strlen(dataToFormat) - 1);

    // on formate la sortie au format json
    formaterMessage(data, sortie);

    // On renvoie le json au client
    renvoie_message(client_socket_fd, sortie);
  }
  else if (strcmp(result.code, "couleurs") == 0)
  {

    // On traite la liste de couleurs sous le format json
    enregistrerCouleursDansFichier("couleur.txt", result.valeurs);

    strcat(dataToFormat, "enregistré");
    strcat(dataToFormat, "\0");
    strncpy(data, dataToFormat, strlen(dataToFormat) - 1);

    // on formate la sortie au format json
    formaterMessage(data, sortie);
    // On renvoie le json au cleint
    renvoie_message(client_socket_fd, sortie);
  }
  else if (strcmp(result.code, "balises") == 0)
  {
    // on traite les balise sous format json
    enregistrerBalisesDansFichier("balise.txt", result.valeurs);
    strcat(dataToFormat, "enregistré");
    strcat(dataToFormat, "\0");
    strncpy(data, dataToFormat, strlen(dataToFormat) - 1);

    // on formate la sortie au format json
    formaterMessage(data, sortie);
    // On renvoie le json au cleint
    renvoie_message(client_socket_fd, sortie);
  }
  // Le code n'est pas reconnu on affiche une erreur
  else
  {
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

  // On extrait le code qui sert a categoriser l'operation
  char code[10];
  sscanf(data, "%s", code);

  // On regarde si on recoit le code message
  if (strcmp(code, "message:") == 0)
  { // On affiche le message
    printf("Message recu: %s\n", data);
    // On renvoie le message au client
    renvoie_message(client_socket_fd, data);
  }
  // on regarde si on recoit un calcul
  if (strcmp(code, "calcule:") == 0)
  {
    printf("Calcul recu: %s\n", data);
    printf("%s", data);

    // on calcule le resultat
    double result = evalOp(data);
    printf("Resultat :  %f\n", result);

    char chaine[50];
    // on affiche le resultat du calcul
    snprintf(chaine, 50, "%f", result);
    // on retourne le resultat au client
    renvoie_message(client_socket_fd, chaine);

  } // on regarde si on recoit le code nom
  if (strcmp(code, "nom:") == 0)
  {
    printf("%s\n", data);
    renvoie_message(client_socket_fd, data);

  } // on regarde si on recoit le code couleurs
  if (strcmp(code, "couleurs:") == 0)
  {
    printf("Couleur recu: %s\n", data);
    enregistrerCouleursDansFichier("couleur.txt", data);
    renvoie_message(client_socket_fd, "Couleur Sauvegardé");

  } // On regarde si on recoit le code balise
  if (strcmp(code, "balises:") == 0)
  {
    printf("Balises recu: %s\n", data);
    enregistrerBalisesDansFichier("balise.txt", data);
    renvoie_message(client_socket_fd, "Balise Sauvegardé");
  }
  // On regarde si le code est couleurs pour afficher l image
  if (strcmp(code, "couleurs:") == 0)
  {
    printf("Image recu: %s\n", data);
    plot(data);

  } // On regarde si on recoit du json et on fait le traitement adequat
  if (strchr(code, '{') != NULL)
  {
    printf("%s \n", data);
    traiterMessageJSON(client_socket_fd, data);
  }
  return (EXIT_SUCCESS);
}

int main()
{
  int socketfd, bind_status;

  struct sockaddr_in server_addr;
  fd_set read_fds, active_fds;
  int client_socket_fds[MAX_CLIENTS] = {0};

  // Création d'une socket
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0)
  {
    perror("Impossible d'ouvrir une socket");
    return -1;
  }

  int option = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  // Détails du serveur (adresse et port)
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

  // Écouter les messages envoyés par le client en boucle infinie
  listen(socketfd, MAX_CLIENTS);

  FD_ZERO(&active_fds);
  FD_SET(socketfd, &active_fds);

  while (1)
  {
    read_fds = active_fds;

    if (select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) < 0)
    {
      perror("select");
      return (EXIT_FAILURE);
    }

    // Vérifier si la socket d'écoute a des données prêtes à être lues (nouvelle connexion)
    if (FD_ISSET(socketfd, &read_fds))
    {
      struct sockaddr_in client_addr;
      unsigned int client_addr_len = sizeof(client_addr);

      // Nouvelle connexion de client
      int client_socket_fd = accept(socketfd, (struct sockaddr *)&client_addr, &client_addr_len);
      if (client_socket_fd < 0)
      {
        perror("accept");
        return (EXIT_FAILURE);
      }

      // On ajoute le nouveau client
      for (int i = 0; i < MAX_CLIENTS; ++i)
      {
        if (client_socket_fds[i] == 0)
        {
          client_socket_fds[i] = client_socket_fd;
          FD_SET(client_socket_fd, &active_fds);
          break;
        }
      }
    }

    // On parcour tous les clients existants pour lire et répondre
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
      if (client_socket_fds[i] > 0 && FD_ISSET(client_socket_fds[i], &read_fds))
      {
        char data[1024];
        memset(data, 0, sizeof(data));

        // Lecture de données envoyées par un client
        int data_size = read(client_socket_fds[i], (void *)data, sizeof(data));

        if (data_size <= 0)
        {
          // Le client a fermé la connexion
          close(client_socket_fds[i]);
          FD_CLR(client_socket_fds[i], &active_fds);
          client_socket_fds[i] = 0;
        }
        else
        // On traite les données envoyé par le client
        {
          recois_envoie_message(client_socket_fds[i], data);
        }
      }
    }
  }

  return 0;
}
