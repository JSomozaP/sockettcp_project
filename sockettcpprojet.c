#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define SERVER_PORT 3001
#define buf_size 255



/*
 * Cette fonction nettoie le texte que tu écris
 * Quand tu appuies sur "Entrée", ça ajoute un retour à la ligne
 * Cette fonction enlève ce retour à la ligne pour que ce soit plus propre
 * C'est comme effacer les petits débordements sur ta feuille !
 */
void noretour(char *str) // Fonction pour supprimer le retour à la ligne d'une chaîne
{
    size_t len = strlen(str); // On compte la longueur du texte
    if (len > 0 && str[len - 1] == '\n')
    {                        // Si le dernier caractère est un retour à la ligne
        str[len - 1] = '\0'; // On le remplace par un marqueur de fin de texte
    }
}

struct player  // Structure pour représenter le joueur
{
    int pv;             // Points de vie (0 = mort)
    char nomjoueur[20]; // Ton nom d'aventurier (maximum 19 lettres)
};

// La réponse HTTP que nous allons envoyer
// Elle contient :
// 1. La ligne de statut HTTP
// 2. Les en-têtes HTTP nécessaires
// 3. Une ligne vide pour séparer les en-têtes du contenu
// 4. Le contenu HTML
const char *response =
    "HTTP/1.1 200 OK\r\n"  // 1. Ligne de statut HTTP
    "Content-Type: text/html\r\n"  // 2. En-tête Content-Type
    "Connection: close\r\n"  // 2. En-tête Connection
    "\r\n"  // 3. Ligne vide
    "<!DOCTYPE html>\n"  // 4. Contenu HTML
    "<html>\n"
    "<head><title>Mon serveur TCP</title></head>\n"
    "<body>\n"
    "<h1>Bienvenue Pouet !</h1>\n"
    "<p>C'est vraiment super interessant !</p>\n"
    "</body>\n"
    "</html>\n";

int main()
{
    // Création du socket serveur
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);  // Créer un socket
    if (server_fd == -1)  // Si la création a échoué
    {
        perror("socket");  // Afficher l'erreur
        return EXIT_FAILURE;  // Quitter le programme
    }

    // Ajouter cette section pour permettre la réutilisation du port
    int opt = 1;  // Valeur pour activer l'option
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)  // Activer l'option
    {
        perror("setsockopt");  // Afficher l'erreur
        close(server_fd); // Fermer le socket
        return EXIT_FAILURE; // Quitter le programme
    }

    // Configuration de l'adresse du serveur
    struct sockaddr_in server_addr = { // Créer une structure sockaddr_in
        .sin_family = AF_INET, // Utiliser IPv4
        .sin_port = htons(SERVER_PORT), // Utiliser le port SERVER_PORT
        .sin_addr.s_addr = INADDR_ANY}; // Utiliser toutes les interfaces réseau disponibles

    // Liaison du socket au port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) // Lier le socket à l'adresse et au port
    {
        perror("bind"); // Afficher l'erreur
        close(server_fd); // Fermer le socket
        return EXIT_FAILURE; // Quitter le programme
    }

    // Mise en écoute du socket
    if (listen(server_fd, 10) == -1) // Mettre le socket en écoute
    { 
        perror("listen"); // Afficher l'erreur
        close(server_fd); // Fermer le socket
        return EXIT_FAILURE; // Quitter le programme
    }

    printf("Serveur en écoute sur le port %d\n", SERVER_PORT);  // Afficher le port du serveur
    printf("Ouvrez votre navigateur et visitez http://localhost:%d\n", SERVER_PORT);  // Afficher l'URL du serveur

    // Boucle principale du serveur
    while (1)
    {
        struct sockaddr_in client_addr;  // Adresse du client
        socklen_t client_len = sizeof(client_addr);   // Taille de l'adresse du client

        // Accepter une nouvelle connexion
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);  // On accepte la connexion
        if (client_fd == -1)  // Si l'acceptation a échoué
        {
            perror("accept"); // Afficher l'erreur
            continue; // On continue à écouter même si une connexion échoue
        }

        // Afficher l'adresse IP du client
        char client_ip[INET_ADDRSTRLEN];  // INET_ADDRSTRLEN est la longueur maximale d'une adresse IPv4 en format texte
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);  // Convertir l'adresse IP en format texte
        printf("Nouvelle connexion depuis %s\n", client_ip);  // Afficher l'adresse IP du client

        // Lire la requête du client (optionnel, on ne l'utilise pas ici)
        char buffer[1024]; // Créer un buffer pour stocker la requête
        recv(client_fd, buffer, sizeof(buffer), 0); // Lire la requête du client

        // Envoyer la réponse HTTP
        send(client_fd, response, strlen(response), 0); // Envoyer la réponse HTTP

        // Fermer la connexion avec ce client
        close(client_fd); // Fermer le socket client
    }

    // Fermer le socket serveur (ce code ne sera jamais atteint dans cet exemple)
    close(server_fd); // Fermer le socket serveur
    return EXIT_SUCCESS; // Quitter le programme
}