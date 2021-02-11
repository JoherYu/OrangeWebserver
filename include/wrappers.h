#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include "event_exception.h"
#include <string.h>

void error_exit(const char* s);
int Epoll_create(int size);
int Socket(int domain, int type, int protocol);
int Fcntl(int fd, int fd_flag, int mode);
int Bind(int fd, const struct sockaddr* addr, socklen_t addr_len);
int	Listen(int fd, int backlog);
int Epoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int Epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
ssize_t Recv(int sockfd, void *buf, size_t len, int flags);
int Open(const char *pathname, int flags);
ssize_t Read(int fd, void *buf, size_t count);

template <typename T> void throw_exception(T ret_value){
    if (ret_value < 0){
	    throw event_exception(strerror(errno));
	}
}
#endif
