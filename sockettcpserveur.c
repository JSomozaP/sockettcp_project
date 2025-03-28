// server.c - un micro-serveur qui accepte une connexion client, attend un message, et y répond
#include <errno.h>  // errno
#include <netdb.h>  // struct sockaddr_in
#include <stdio.h>  // printf, fprintf
#include <string.h>  // memset, strerror, strlen
#include <sys/types.h> // socket, bind, listen, accept, recv, send
#include <sys/socket.h>  // socket, bind, listen, accept, recv, send
#include <netinet/in.h> // struct sockaddr_in
#include <unistd.h>  // close

#define PORT 4242  // le port de notre serveur
#define BACKLOG 10 // nombre max de demandes de connexion

int main(void)  // pas besoin d'arguments pour notre serveur
{
    printf("---- SERVER ----\n\n");  // pour savoir qui est qui 
    struct sockaddr_in sa;  // pour l'adresse et le port du serveur
    int socket_fd;  // le file descriptor de la socket du serveur
    int client_fd;  // le file descriptor de la socket du client
    int status;  // pour les retours de fonctions
    struct sockaddr_storage client_addr;  // pour l'adresse du client
    socklen_t addr_size;  // pour la taille de l'adresse du client
    char buffer[BUFSIZ];  // pour stocker les messages reçus
    int bytes_read;  // pour le nombre de bytes reçus

    // on prépare l'adresse et le port pour la socket de notre serveur
    memset(&sa, 0, sizeof sa);  // on met tout à zéro
    sa.sin_family = AF_INET; // IPv4
    sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1, localhost
    sa.sin_port = htons(PORT); // le port sur lequel on écoute

    // on crée la socket, on a lit et on écoute dessus
    socket_fd = socket(sa.sin_family, SOCK_STREAM, 0);  // on crée la socket
    if (socket_fd == -1) {  // si la création a échoué
        fprintf(stderr, "socket fd error: %s\n", strerror(errno));  // on affiche l'erreur
        return (1);
    }
    printf("Created server socket fd: %d\n", socket_fd);  // on affiche le file descriptor de la socket

    status = bind(socket_fd, (struct sockaddr *)&sa, sizeof sa);  // on lie la socket à l'adresse et au port
    if (status != 0) {  // si la liaison a échoué
        fprintf(stderr, "bind error: %s\n", strerror(errno));  // on affiche l'erreur
        return (2);
    }
    printf("Bound socket to localhost port %d\n", PORT);  // on affiche le port auquel on est lié

    printf("Listening on port %d\n", PORT);  // on affiche qu'on écoute sur le port
    status = listen(socket_fd, BACKLOG);  // on écoute sur la socket
    if (status != 0) {  // si l'écoute a échoué
        fprintf(stderr, "listen error: %s\n", strerror(errno));  // on affiche l'erreur
        return (3);
    }

    // on accepte une connexion entrante
    addr_size = sizeof client_addr; // on met la taille de l'adresse du client
    client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &addr_size); // on accepte la connexion
    if (client_fd == -1) { // si l'acceptation a échoué
        fprintf(stderr, "client fd error: %s\n", strerror(errno));  // on affiche l'erreur
        return (4);
    }
    printf("Accepted new connection on client socket fd: %d\n", client_fd);

    // on recoit un message via la socket client
    bytes_read = 1;
    while (bytes_read >= 0) {
        printf("Reading client socket %d\n", client_fd);
        bytes_read = recv(client_fd, buffer, BUFSIZ, 0);
        if (bytes_read == 0) {
            printf("Client socket %d: closed connection.\n", client_fd);
            break ;
        }
        else if (bytes_read == -1) {
            fprintf(stderr, "recv error: %s\n", strerror(errno));
            break ;
        }
        else {
            // Si on a bien reçu un message, on va l'imprimer
            // puis renvoyer un message au client
            char *msg = "Got your message.";
            int msg_len = strlen(msg);
            int bytes_sent;

            buffer[bytes_read] = '\0';
            printf("Message received from client socket %d: \"%s\"\n", client_fd, buffer);

            bytes_sent = send(client_fd, msg, msg_len, 0);
            if (bytes_sent == -1) {
                fprintf(stderr, "send error: %s\n", strerror(errno));
            }
            else if (bytes_sent == msg_len) {
                printf("Sent full message to client socket %d: \"%s\"\n", client_fd, msg);
            }
            else {
                printf("Sent partial message to client socket %d: %d bytes sent.\n", client_fd, bytes_sent);
            }
        }
    }

    printf("Closing client socket\n");
    close(client_fd);
    printf("Closing server socket\n");
    close(socket_fd);

    return (0);
}