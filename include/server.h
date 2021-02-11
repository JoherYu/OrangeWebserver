#ifndef SERVER_H
#define SERVER_H

#include <sys/epoll.h>

template <int MAX_EVENT_NUMBER>
class server
{

public:
	server(int port);
	void init();
	void start();

private:
	const int PORT;
	int epoll_fd;
	struct epoll_event events[MAX_EVENT_NUMBER + 1];
};

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <stdio.h>
#include <string.h>
#include "event_data.h"
#include "server.h"
#include "wrappers.h"

template <int MAX_EVENT_NUMBER>
server<MAX_EVENT_NUMBER>::server(int port) : PORT(port){};

template <int MAX_EVENT_NUMBER>
void server<MAX_EVENT_NUMBER>::init()
{

	epoll_fd = Epoll_create(MAX_EVENT_NUMBER + 1);
	event_data::set_root(epoll_fd);
	int lfd = Socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in sin;
	Fcntl(lfd, F_SETFL, O_NONBLOCK);
	memset(&sin, 0, sizeof(sin));

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(PORT);

	Bind(lfd, (struct sockaddr *)&sin, sizeof(sin));
	Listen(lfd, 20);

	event_data *lnode = new event_data(lfd, acceptconn); // delete isn't necessary
	lnode->mounted(EPOLLIN);
}

template <int MAX_EVENT_NUMBER>
void server<MAX_EVENT_NUMBER>::start()
{
	while (1)
	{
		int fd_count = Epoll_wait(epoll_fd, events, MAX_EVENT_NUMBER + 1, 1000);
		for (int i = 0; i < fd_count; i++)
		{
			event_data *node = (event_data *)events[i].data.ptr;
			node->process();
		}
	}
}

#endif
