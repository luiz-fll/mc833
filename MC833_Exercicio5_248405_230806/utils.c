#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include "utils.h"


int Socket(int family, int type, int protocol) {
    int sockfd = socket(family, type, protocol);
    if ((sockfd) < 0) {
        perror("socket");
        exit(1);
    }
    return sockfd;
}

int Bind(int fd, const struct sockaddr *addr, socklen_t len) {
    int r = bind(fd, addr, len);
    if (r < 0) {
        perror("bind");
        Close(fd);
        exit(1);
    }
    return r;
}

int Listen(int sockfd, int backlog) {
    int r;
    if ((r = listen(sockfd, backlog)) == -1) {
        perror("listen");
        Close(sockfd);
        exit(1);
    }
    return r;
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int connfd = accept(sockfd, addr, addrlen);
    if (connfd == -1) {
        perror("accept");
    }
    return connfd;
}

pid_t Fork(void) {
    int pid = fork();
    if (pid < 0 ) {
        perror("fork");
    }
    return pid;
}

int Close(int fd) {
    int r;
    if ((r = close(fd)) != 0) {
        perror("close");
    }
    return r;
}

int Connect(int fd, const struct sockaddr *addr, socklen_t len) {
    int n;
    if ((n = connect(fd, addr, len))) {
        perror("connect");
    }
    return n;
}

ssize_t Read(int fd, void *buf, size_t nbytes) {
    int n = read(fd, buf, nbytes);
    if (n < 0) {
        perror("read");
    }
    return n;
}

ssize_t Write(int fd, const void *buf, size_t n) {
    int nbytes = write(fd, buf, n);
    if (nbytes < 0) {
        perror("write");
    }
    return nbytes;
}

__sighandler_t Signal(int sig, __sighandler_t handler) {
    __sighandler_t r = signal(sig, handler);
    if (r == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    return r;
}

int Select(int nfds, fd_set *rfds, fd_set *wfds, fd_set *efds, struct timeval *tv) {
    int retval = select(nfds, rfds, wfds, efds, tv);
    if (retval == -1) {
        perror("select");
    }
    return retval;
}