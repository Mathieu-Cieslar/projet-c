# Bibliothèques
 <math.h> 
 <netinet/in.h>
 <sys/types.h> 
  <sys/socket.h> 
  <signal.h> 
  <stdio.h> 
  <stdlib.h> 
  <string.h> 
  <unistd.h>
  <sys/select.h>
 <stdbool.h>

# Difficulté
* Pour moi la principale difficultés a été le manque de temps ainsi que la partie multi-clients serveur 

# Commentaires
* Le code contient toutes les parties de la 1 à la 5, toutes les fonctionnalités ont été gardée ainsi on peut envoyer des message simple, envoyer message json et avoir un fonctionnement multi client (uniquement en message json)

* Comment utiliser notre programme :
* Serveur
Tout d'abord lancer programme `./serveur` celui ci ne prend aucun parametre.

Ensuite il faut lancer un client.

# Partie 1 a 4

* Client
Pour lancer un client il suffite de taper `./client nom`
* Attention le client prend toujours au moins un argument !
Voici les differntes commandes que vous pouvez lancer : 
- `./client nom`
- `./client message`
- `./client calcule`
- `./client couleurs`
- `./client balises`

Ces commandes sont fait pour passer des messages simples entre le client et le serveur.

La commande `./client couleurs` va demander la liste des couleurs a l'utilisateur, puis les enregistrer dans un fichier couleur.txt et ensuite constuire le graphique grace a ces couleurs.

La commande `./client balises` va demander la liste des balises a l'utilisateur, puis les enregistrer dans un fichier balises.txt .

On peut egalement envoyer un fichier bmp qui agira une liste de couleur.
On peut specifier le nombre max de couleur en ajoutant un parametre a la fin :
- `./client images.bmp 5`

# En JSON 

On peut egalement envoyer ces messages au serveur en json en ajoutant un parametre json a la fin de chaque commande : 
- `./client nom json`
- `./client message json`
- `./client calcule json`
- `./client couleurs json`
- `./client balises json`

# Partie 5

Dans la partie 5 le multi client a été introduit et donc pour lancer le client il faut entrer la commande : 
- `./client`

Cette commande ne prend pas de paramètre et va tourner en boucle tant que l'utilisateur n'a pas chosit de quitter le programme 

Une fois le client lancé l'utilisateur à doit saisir une action et rentrer les mêmes données que pour les commandes précdentes.

Durant cette commande tous les message sont envoyés et réceptionné en JSON puis décodé pour être affiché a l'utilisateur.


