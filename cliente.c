// cliente_base.c — conecta, lê banner do servidor e fecha
// Compilação: gcc -Wall cliente_base.c -o cliente
// Uso: ./cliente [IP] [PORT]

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils.h"

#define MAXLINE 4096

int main(int argc, char **argv) {
    int    sockfd;
    struct sockaddr_in servaddr;

    // IP/PORT (argumentos ou server.info)
    char ip[INET_ADDRSTRLEN] = "127.0.0.1";
    unsigned short port = 0;

    if (argc >= 2) strncpy(ip, argv[1], sizeof(ip)-1);
    if (argc >= 3) port = (unsigned short)atoi(argv[2]);

    if (port == 0) {
        FILE *f = fopen("server.info", "r");
        if (f) {
            char line[128]; int got_p = 0;
            while (fgets(line, sizeof(line), f)) {
                (void)sscanf(line, "IP=%127s", ip);        // lê IP se houver, sem flag
                if (sscanf(line, "PORT=%hu", &port) == 1) got_p = 1;
            }
            fclose(f);
            if (!got_p) port = 0;
        }
        if (port == 0) {
            fprintf(stderr, "Uso: %s <IP> [PORT] (ou forneça server.info)\n", argv[0]);
            return 1;
        }

    }

    // socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        return 1;
    }

    // connect
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(port);
    if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0) {
        perror("inet_pton error");
        close(sockfd);
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("connect error");
        close(sockfd);
        return 1;
    }

    // MODIFICACAO: 4-tupla de dados
    struct sockaddr_in bound;
    socklen_t blen = sizeof(bound);
    if (getsockname(sockfd, (struct sockaddr*)&bound, &blen) == 0) {
        unsigned short p = ntohs(bound.sin_port);
        char *ip_string = inet_ntoa(bound.sin_addr);
        printf("local: %s:%u\n", ip_string, p);
    }

    // MODIFICACAO: Adiciona funcionalidade de mensagens
    char client_msg[MAXLINE];
    printf("Digite uma mensagem: ");
    if (fgets(client_msg, sizeof(client_msg), stdin) == NULL) { // Verificacao de erro
        perror("fgets error");
        close(sockfd);
        return 1;
    }
    (void)write(sockfd, client_msg, strlen(client_msg));

    // lê e imprime o banner (uma leitura basta neste cenário)
    char banner[MAXLINE + 1];
    ssize_t n = read(sockfd, banner, MAXLINE);
    if (n > 0) {
        banner[n] = 0;
        fputs(banner, stdout);
        fflush(stdout);
    }

    close(sockfd);
    return 0;
}
