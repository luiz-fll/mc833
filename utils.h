#ifndef UTILS_H
#define UTILS_H

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

int Socket(int family, int type, int protocol);

int Bind(int fd, const struct sockaddr *addr, socklen_t len);

int Listen(int sockfd, int backlog);

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

pid_t Fork(void);

int Close(int fd);

ssize_t Read(int fd, void *buf, size_t nbytes);

ssize_t Write(int fd, const void *buf, size_t n);

#endif
