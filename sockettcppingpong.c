#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


//#define SOCKET_PORT 3000
#define SERVER_PORT 3001
#define CLIENT_PORT 4001

char buf[BUFSIZ];

int main(){

    /**
     * SOCKET
     * je crée le socket serveur
     */
    int server_fd= socket(AF_INET,SOCK_STREAM,0);perror("socket");
    if(server_fd == -1) return EXIT_FAILURE;
    /**
     * BIND
     * je relie le socket à un port et une ip avec la fonction bind()
     */

    struct sockaddr_in socket_addr = {
        .sin_addr.s_addr = INADDR_ANY,
        .sin_family = AF_INET,
        .sin_port =htons(SERVER_PORT)
    };
    int error = bind(server_fd,(struct sockaddr*)&server_addr,sizeof server_addr);perror("bind");
    if(error == -1) { close(server_fd); return EXIT_FAILURE; }

    /**
     * LISTEN obligatoire
     */
    error = listen(server_fd,BUFSIZ);perror("listen");
    if (error == -1){close(server_fd); return EXIT_FAILURE; }

    printf("Serveur en écoute sur le port : %d\n", SERVER_PORT);

    /**
     * ACCEPT
     * J'attend q'un clien se connecte grâce à la fonction bloquante accept()
     * accept renvoi le client accepte ou -1 en cas d'erreur
     */
    while(1){
    struct sockaddr_in client_addr;
    socklen_t len;
    int client_fd = accept(server_fd,(struct sockaddr*)&client_addr,&len);

    //LE PROGRAMME EST EN PAUSE ET ATTEND UN CLIENT ...

    perror("accept");
    if(client_fd == -1){close(client_fd);close(server_fd); return EXIT_FAILURE;}

    //Prêt à communiquer avec le client

    return EXIT_SUCCESS;
}