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
* Pour moi la principale difficultés a été le manque de temps ainsi que la partie multi-clients serveur avec les tests car nous n'avons pas bien compris ce qui était demandé notemment quel partie il fallait tester car les fonction ne pour traitre les message ne retournait rien et ecrivait juste dans la socket 

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


# En JSON 

On peut egalement envoyer ces messages au serveur en json en ajoutant un parametre json a la fin de chaque commande : 
- `./client nom json`
- `./client message json`
- `./client calcule json`
- `./client couleurs json`
- `./client balises json`

## Les commandes en details

La commande `./client calcule json`  va demander un calcul à l'utilisateur, celui-ci devra soit saisir un calcule simple avec un operateur (+-/*) et 2 nombre séparé par des espaces : `+ 5 2`  soit saisir une fonction avec une liste infini de nombre exemple : `moyenne 5 8 9 56 4 2 3 7` .

La commande `./client message json`  va demander un message à l'utilisateur, celui-ci devra saisir un message par exemple : `test` .

La commande `./client couleurs json` va demander la liste des couleurs a l'utilisateur, celui-ci devra saisir une liste de la forme `3,#ehhhh,#chheee,#fhhhhh`, elles seront enregistrer dans un fichier couleur.txt.

La commande `./client balises json` va demander la liste des balises a l'utilisateur, celui-ci devra saisir une liste de la forme `3,#arbre,#chat,#chien`, elles seront enregistrer dans un fichier balises.txt .

On peut egalement envoyer un fichier bmp qui agira une liste de couleur.
On peut specifier le nombre max de couleur en ajoutant un parametre a la fin :
- `./client images.bmp 5`


# Partie 5

### Multi-clients

Dans la partie 5 le multi client a été introduit et donc pour lancer le client il faut entrer la commande : 
- `./client`

Cette commande ne prend pas de paramètre et va tourner en boucle tant que l'utilisateur n'a pas chosit de quitter le programme 

Une fois le client lancé l'utilisateur à doit saisir une action et rentrer les mêmes données que pour les commandes précdentes.

Durant cette commande tous les message sont envoyés et réceptionné en JSON puis décodé pour être affiché a l'utilisateur.


### Test 

Nous avons testé les fonction de calcul et les fonction attrayant au JSON (verfication de la conformité et encodage)

Pour lancer les tests il suffit de lancer la commande :
`./client testes`

