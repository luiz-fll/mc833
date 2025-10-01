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

void print_ip_port(char *text, struct sockaddr_in *address) {
    unsigned short p = ntohs(address->sin_port);
    char *ip_string = inet_ntoa(address->sin_addr);
    printf("%s: %s:%u\n", text, ip_string, p);
}

int main(int argc, char **argv) {
    int    sockfd;
    struct sockaddr_in servaddr;

    // IP/PORT (argumentos ou server.info)
    char ip[INET_ADDRSTRLEN] = "127.0.0.1";
    unsigned short port = 0;

    if (argc >= 2) strncpy(ip, argv[1], sizeof(ip)-1);
    if (argc >= 3) port = (unsigned short)atoi(argv[2]);

    // socket
    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    // connect
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(port);
    if (inet_pton(AF_INET, ip, &servaddr.sin_addr) < 0) {
        perror("inet_pton error");
        Close(sockfd);
        return 1;
    }

    Connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    // MODIFICACAO: 4-tupla de dados
    struct sockaddr_in bound;
    socklen_t blen = sizeof(bound);
    if (getsockname(sockfd, (struct sockaddr*)&bound, &blen) == 0) {
        print_ip_port("local", &bound);
    }
    print_ip_port("remoto", &servaddr);

    char *s = "GET / HTTP/1.0\r\n"
              "Host: teste\r\n"
              "\r\n";
    Write(sockfd, s, 31);

    // lê e imprime o banner (uma leitura basta neste cenário)
    char response[MAXLINE + 1];
    ssize_t n = Read(sockfd, response, MAXLINE);
    write(STDOUT_FILENO, response, n);
    Close(sockfd);

    return 0;
}
