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

#define LISTENQ      10
#define MAXDATASIZE  256

int main(void) {
    int listenfd, connfd;
    struct sockaddr_in servaddr;

    // socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return 1;
    }

    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
    servaddr.sin_port        = htons(0); // MODIFICACAO: Automatizar escolha de portas

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind");
        close(listenfd);
        return 1;
    }

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
    if (listen(listenfd, LISTENQ) == -1) {
        perror("listen");
        close(listenfd);
        return 1;
    }

    // laço: aceita clientes, envia banner e fecha a conexão do cliente
    for (;;) {
        connfd = accept(listenfd, NULL, NULL);
        if (connfd == -1) {
            perror("accept");
            continue; // segue escutando
        }

        // MODIFICACAO: Recebe mensagem do cliente
        char client_msg[MAXDATASIZE];
        ssize_t bytes_read = read(connfd, client_msg, sizeof(client_msg));

        if (bytes_read > 0) {
            // Garante que a string seja terminada em nulo
            client_msg[bytes_read] = '\0';
            printf("[CLI MSG] %s", client_msg);
        } else if (bytes_read == 0) {
            // Conexao fechada pelo cliente (nao tem como enviar 0 bytes)
            printf("[SERVIDOR] Cliente desconectou.\n");
        } else {
            // Erro na leitura
            perror("read error");
        }

        // envia banner "Hello + Time"
        char buf[MAXDATASIZE];
        time_t ticks = time(NULL); // ctime() já inclui '\n'
        snprintf(buf, sizeof(buf), "Hello from server!\nTime: %.24s\r\n", ctime(&ticks));
        (void)write(connfd, buf, strlen(buf));

        close(connfd); // fecha só a conexão aceita; servidor segue escutando
    }

    return 0;
}
