#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>

#include "utils.h"

#define LISTENQ      10
#define MAXDATASIZE  256

void answer(int connfd) {
    const char *s = 
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 91\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<html><head><title>MC833</title</head><body><h1>MC833 TCP"
        "Concorrente </h1></body></html>";
    Write(connfd, s, strlen(s));
}

void error(int connfd) {
    const char *s = "404 Not Found\r\n";
    Write(connfd, s, strlen(s));
}

void sigchld(int signo) {
    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        printf("child %d terminated\n", pid);
    }
}

int main(int argc, char **argv) {

    Signal(SIGCHLD, sigchld);

    int port = 0;
    int delay = 0;
    int backlog = LISTENQ;
    if (argc >= 2) {
        port = atoi(argv[1]);
    }
    if (argc >= 3) {
        backlog = atoi(argv[2]);
    }
    if (argc >= 4) {
        delay = atoi(argv[3]);
    }

    int listenfd, connfd;
    struct sockaddr_in servaddr;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
    servaddr.sin_port        = htons(port);

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    // Descobrir porta real e divulgar em arquivo server.info
    struct sockaddr_in bound; socklen_t blen = sizeof(bound);
    if (getsockname(listenfd, (struct sockaddr*)&bound, &blen) == 0) {
        unsigned short p = ntohs(bound.sin_port);
        char *ip_string = inet_ntoa(bound.sin_addr); // MODIFICACAO: Coleta endereco IP
        printf("remoto: %s:%u\n", ip_string, p); // MODIFICACAO: 4-tupla de dados
        printf("[SERVIDOR] Escutando em 127.0.0.1:%u\n", p);
        FILE *f = fopen("server.info", "w");
        if (f) { fprintf(f, "IP=127.0.0.1\nPORT=%u\n", p); fclose(f); }
        fflush(stdout);
    }
    // listen
    Listen(listenfd, backlog);

    // laço: aceita clientes, envia banner e fecha a conexão do cliente
    for (;;) {
        struct sockaddr_in client_addr;
        socklen_t s = sizeof(client_addr);
        connfd = Accept(listenfd, (struct sockaddr *)&client_addr, &s);
        pid_t pid = Fork();
        if (pid > 0) {
            Close(connfd);
        } else {
            sleep(delay);
            Close(listenfd);
            char request_buf[512];
            char *ptr = request_buf;
            int n = Read(connfd, request_buf, 511);
            unsigned short p = ntohs(client_addr.sin_port);
            char *ip_string = inet_ntoa(client_addr.sin_addr);
            request_buf[n] = '\0';
            printf("%s:%u\n%s", ip_string, p, request_buf);
            if (strncmp(request_buf, "GET / ", 6) != 0) {
                printf("a");
                error(connfd);
            } else {
                ptr += 6;
                if (strncmp(ptr, "HTTP/1.0", 8) == 0) {
                    answer(connfd);
                } else if (strncmp(ptr, "HTTP/1.1", 8) == 0) {
                    answer(connfd);
                } else {
                    printf("b");
                    error(connfd);
                }
            }
            Close(connfd);
            return 0;
        }
    }
    return 0;
}
