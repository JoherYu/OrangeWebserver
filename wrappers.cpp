#include "wrappers.h"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <string>
#include <cstdio>

void error_exit(const char *s)
{
	perror(s);
	exit(-1);
}

int Epoll_create(int size)
{
	int ret = epoll_create(size);
	if (ret < 0)
	{
		error_exit("epoll_create error: ");
	}
	return ret;
}

int Socket(int domain, int type, int protocol)
{
	int ret = socket(domain, type, protocol);
	if (ret < 0)
	{
		error_exit("socket error: ");
	}
	return ret;
}

int Fcntl(int fd, int fd_flag, int mode)
{
	int ret = fcntl(fd, fd_flag, mode);
	if (ret < 0)
	{
		error_exit("fcntl error: ");
	}
	return ret;
}

int Bind(int fd, const struct sockaddr *addr, socklen_t addr_len)
{
	int ret = bind(fd, addr, addr_len);
	if (ret < 0)
	{
		error_exit("bind error: ");
	}
	return ret;
}

int Listen(int fd, int backlog)
{
	int ret = listen(fd, backlog);
	if (ret < 0)
	{
		error_exit("listen error: ");
	}
	return ret;
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int ret = 0;
	while ((ret = accept(sockfd, addr, addrlen)) < 0)
	{
		if ((errno == ECONNABORTED) || (errno == EINTR))
		{
			continue;
		}
		else
		{
			error_exit("accept error: ");
		}
	}
	return ret;
}

int Epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
	int ret = epoll_ctl(epfd, op, fd, event);
	if (ret < 0)
	{
		perror("epoll_ctl error");
		Close(fd, "");
	}

	return ret;
}

ssize_t Recv(int sockfd, char *buf, size_t len, int flags)
{
	ssize_t ret = recv(sockfd, buf, len, flags);
	if (ret < 0)
	{
		if ((errno == EAGAIN) || (errno == EINTR))
		{
			//todo: time out
			return 1; // for get_line: continue
		}
		else
		{
			throw_exception(ret);
		}
	}

	return ret;
}

int Open(const char *pathname, int flags)
{
	int ret = open(pathname, flags);
	if (ret < 0)
		throw_exception(ret);
	return ret;
}

ssize_t Read(int fd, void *buf, size_t count)
{
	ssize_t ret = read(fd, buf, count);
	if (ret < 0)
		throw_exception(ret);
	return ret;
}

int Write(int fd, char *content, string fd_name)
{
	int ret = write(fd, content, string(content).size());
	if (ret < 0)
	{
		perror(("write " + fd_name + "error ").c_str());
	}
	return ret;
}

void Close(int fd, string fd_name)
{
	int ret = close(fd);
	if (ret < 0)
	{
		perror(("close " + fd_name + "error ").c_str());
	}
}

int Epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
	int ret = epoll_wait(epfd, events, maxevents, timeout);
	if (ret < 0)
	{
		error_exit("epoll_wait error: ");
	}
	return ret;
}

mode_t Stat(const char *file_path)
{
	struct stat st;
	int ret = stat(file_path, &st);
	if ((ret < 0))
	{
		perror("stat error:");
		throw_exception(4, "resouce is missing", Static);
	}
	return st.st_mode;
}

void Pipe(int *const fds)
{
	int ret = pipe(fds);
	if (ret < 0)
	{
		perror("pipe error:");
		throw_exception(4, "server error", Dynamic);
	}
}

int Wait()
{
	int status;
	int ret = wait(&status);
	if (ret == -1)
	{
		perror(("process " + to_string(getpid()) + " wait error ").c_str());
		throw_exception(5, "server error", Dynamic);
	}

	return WEXITSTATUS(status);
}
