#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>   // Ajout pour time()

#define SERVER_PORT 3002
#define CLIENT_PORT 4001

int main() {
    // Ajout de la déclaration du pseudo
    char pseudo[50];
    printf("Entrez votre pseudo : ");
    fgets(pseudo, sizeof(pseudo), stdin);
    pseudo[strcspn(pseudo, "\n")] = 0;  // Enlève le retour à la ligne

    

    /**
     * socket
     * Je crée le socket client
     */
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    perror("socket");
    if(client_fd == -1) return EXIT_FAILURE;

    /**
     * bind
     * Je relie le socket à un port et une ip avec la fonction bind()
     */
    struct sockaddr_in client_addr = {
        .sin_addr.s_addr = INADDR_ANY,
        .sin_family = AF_INET,
        .sin_port = htons(CLIENT_PORT)
    };
    int error = bind(client_fd, (struct sockaddr*)&client_addr, sizeof client_addr);
    perror("bind");
    if(error == -1) { close(client_fd); return EXIT_FAILURE; }

    /**
     * connect
     * Je connecte mon socket client au socket server situé en 127.0.0.1:SERVER_PORT
     */
    struct sockaddr_in server_addr = {
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT)
    };
    error = connect(client_fd, (struct sockaddr*)&server_addr, sizeof server_addr);
    perror("connect");
    if(error == -1) { close(client_fd); return EXIT_FAILURE; }

    // SOCKET CLIENT PRET A COMMUNIQUER !
    while(error != -1) {
        // Je demande le message à l'utilisateur
        char message[BUFSIZ];
        memset(message, 0, BUFSIZ);
        fgets(message, BUFSIZ, stdin);

        // Je formate le message dans un buffer
        char buf[BUFSIZ];
        memset(buf, 0, BUFSIZ);
        
        // Calcul de la taille maximale disponible pour le message
        size_t timestamp_len = 20;  // [timestamp] ~12-15 chars
        size_t pseudo_len = strlen(pseudo);
        size_t prefix_len = timestamp_len + pseudo_len + 3; // +3 pour "]: "
        size_t max_msg_len = BUFSIZ - prefix_len - 1; // -1 pour le \0
        
        // Tronquer le message si nécessaire
        if (strlen(message) > max_msg_len) {
            message[max_msg_len] = '\0';
        }

        // Formatage sécurisé du message
        snprintf(buf, BUFSIZ, "[%ld]%s: %.*s", time(NULL), pseudo, (int)max_msg_len, message);

        // J'envoie le message au serveur du salon de discussion
        error = send(client_fd, buf, strlen(buf), 0);
    }

    close(client_fd);
    return EXIT_SUCCESS;
}