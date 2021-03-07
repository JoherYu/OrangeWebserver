#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <arpa/inet.h>
#include <errno.h>

#include "event_exception.h"

#include <cstdlib>
#include <cstring>

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

template <typename T>
void throw_exception(int fd, int error_code, T message, response_type type, string func_name) // todo event store type and move to event_exception
{
	string error_message = (std::is_same<T, int>::value) ? strerror(errno) : message;
	throw event_exception(fd, error_code, error_message, type, func_name);
	/* 	if 
	{
		error_message
	}
	
    if (){
	    throw event_exception(strerror(errno));
	} */
}

template <typename T>
void throw_exception(T ret_value)
{
	throw event_exception(strerror(errno));
}
#endif
