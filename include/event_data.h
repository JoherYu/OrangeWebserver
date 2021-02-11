#ifndef EVENT_DATA_H
#define EVENT_DATA_H

#include <sys/epoll.h>
#include <time.h>
#include <memory>
#include "http.h"

using namespace std;

const int BUFLEN = 4096;

class event_data
{

	friend void acceptconn(event_data &node);
	friend void recvdata(event_data &node);
	friend void senddata(event_data &node);

public:
	event_data(int fd, void (*call_back)(event_data &));
	event_data(int fd, void (*call_back)(event_data &), shared_ptr<http> message);
	void mounted(int n_event_name);
	void unmounted();
	void process();
	static void set_root(int root_fd);

private:
	int fd;
	int event_name;
	void (*call_back)(event_data &);
	shared_ptr<http> message; // todo:repalce with template
	int status;
	int len;
	time_t last_active;
	static int epoll_root;
};

void acceptconn(event_data &node);
void recvdata(event_data &node);
void senddata(event_data &node);

#endif
