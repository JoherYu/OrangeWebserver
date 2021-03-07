#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <arpa/inet.h>
#include <errno.h>

#include "event_exception.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

void error_exit(const char *s);
int Epoll_create(int size);
int Socket(int domain, int type, int protocol);
int Fcntl(int fd, int fd_flag, int mode);
int Bind(int fd, const struct sockaddr *addr, socklen_t addr_len);
int Listen(int fd, int backlog);
int Epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int Epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
ssize_t Recv(int sockfd, char *buf, size_t len, int flags);
int Open(const char *pathname, int flags);
ssize_t Read(int fd, void *buf, size_t count);
int Write(int fd, char *content, string fd_name);
void Close(int fd, string fd_name);
mode_t Stat(const char *file_path);
void Pipe(int *const fds);
int Wait();

#endif
