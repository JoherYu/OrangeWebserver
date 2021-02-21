#include "server.h"

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "event_data.h"
#include "wrappers.h"
#include "utils.h"

#include <cstdio>
#include <string>

server::server(int port, int max_event_number) : PORT(port), MAX_EVENT_NUMBER(max_event_number)
{
	events = new epoll_event[max_event_number + 1];
};

void server::init()
{
	cout << "[" << get_time() << "]"
		 << "initializing server" << endl;

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

	cout << "[" << get_time() << "]"
		 << "server initialization complete" << endl;
}

void server::start()
{
	cout << "[" << get_time() << "]"
		 << "server start" << endl;
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