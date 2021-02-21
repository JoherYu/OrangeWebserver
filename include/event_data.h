#ifndef EVENT_DATA_H
#define EVENT_DATA_H

#include "http.h"
#include "http_response.h"

#include <sys/epoll.h>

#include <ctime>
#include <memory>


using namespace std;

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
	shared_ptr<http> message; 
	int status;
	int len;
	time_t last_active;
	static int epoll_root;

	static void error_mounted(int fd, shared_ptr<http_response> response, int error_code, const string& error_descp, const string& error_info);

};

inline void event_data::set_root(int root_fd)
{
	epoll_root = root_fd;
};

void acceptconn(event_data &node);
void recvdata(event_data &node);
void senddata(event_data &node);

#endif
