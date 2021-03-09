#ifndef SERVER_H
#define SERVER_H

#include <sys/epoll.h>
#include <iostream>

#include <threadpool.h>

using namespace std;

class server
{

public:
	server(int port, int max_event_number);
	void init();
	void start(threadpool &pool);

private:
	const int PORT;
	const int MAX_EVENT_NUMBER;
	int epoll_fd;
	struct epoll_event *events;
	;
};

#endif
