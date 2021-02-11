#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <arpa/inet.h>

void error_exit(const char* s);
int Epoll_create(int size);
int Socket(int domain, int type, int protocol);
int Fcntl(int fd, int fd_flag, int mode);
int Bind(int fd, const struct sockaddr* addr, socklen_t addr_len);
int	Listen(int fd, int backlog);
int Epoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout);

#endif
