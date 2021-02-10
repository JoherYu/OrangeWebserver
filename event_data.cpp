#include "event_data.h"
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <memory.h>
#include <sys/stat.h>
#include <stdio.h>
#include <iostream>
#include "http.h"
#include "http_response.h"
#include "utils.h"

using std::cout;

int event_data::epoll_root = 0;

event_data::event_data(int fd, void (*call_back)(event_data &)) : fd(fd), call_back(call_back), event_name(0), status(0), len(0),
																  message(NULL)
{
	last_active = time(NULL);
}

event_data::event_data(int fd, void (*call_back)(event_data &), shared_ptr<http> message) : fd(fd), call_back(call_back), event_name(0), status(0), len(0),
																							message(message)
{
	last_active = time(NULL);
}

void event_data::process()
{
	(*call_back)(*this);
}

void event_data::mounted(int n_event_name)
{
	struct epoll_event epv = {0, {0}};
	int op;
	epv.data.ptr = this;
	epv.events = event_name = n_event_name;

	if (status == 0)
	{
		op = EPOLL_CTL_ADD;
		status = 1;
	}

	epoll_ctl(epoll_root, op, fd, &epv);
}

void event_data::unmounted()
{

	if (status != 1)
		return;

	status = 0;
	epoll_ctl(epoll_root, EPOLL_CTL_DEL, fd, NULL);
}

void event_data::set_root(int root_fd)
{
	epoll_root = root_fd;
};

void acceptconn(event_data &node)
{
	struct sockaddr_in cin;
	socklen_t len = sizeof(cin);
	int cfd = accept(node.fd, (struct sockaddr *)&cin, &len);
	fcntl(cfd, F_SETFL, O_NONBLOCK);
	event_data cnode(cfd, recvdata);
	cnode.mounted(EPOLLIN);
	return;
};

void recvdata(event_data &node)
{
	char buf[1024] = {0};
	int len = get_line(node.fd, buf, sizeof(buf));

	char method[12], path[1024], protocol[12];
	sscanf(buf, "%[^ ] %[^ ] %[^ \n]", method, path, protocol);

	recv(node.fd, buf, sizeof(buf), 0);

#if 0	
	http_request request(method, url, protocol);
    if(method == "POST"){
		buf = {0};	
	    recv(node.fd, buf, sizeof(buf), 0);
		// todo extract data
	}
#endif

	node.unmounted();

	char *file = path + 1;
	struct stat st;
	int ret = stat(file, &st);
	if (ret == -1)
	{
		return;
	}

	shared_ptr<http_response> response;
	if (S_ISDIR(st.st_mode))
	{
		//todo
	}
	else if (S_ISREG(st.st_mode))
	{
		response = make_shared<http_response>(200, "OK", protocol, file, st.st_size);
		ret = open_file(file, *response);
		cout << ret;
		if (ret != 0)
		{
			response->set_status_code(500);
			response->set_status_descp("Please Try Again");
		}
	}

	if (len > 0)
	{
		event_data *w_node = new event_data(node.fd, senddata, response);
		w_node->mounted(EPOLLOUT);
	}
	else if (len == 0)
	{
		close(node.fd);
	}
	else
	{
		close(node.fd);
	}
};

void senddata(event_data &node)
{
	auto response = node.message;
	string response_buf =
		response->get_protocol() + " " + response->get_status_code() + " " + response->get_headers() +
		"\r\n" +
		response->get_data();

	int ret = send(node.fd, response_buf.data(), response_buf.size(), 0);

	node.unmounted();
	delete &node;
	if (ret > 0)
	{
		event_data r_node = event_data(node.fd, recvdata);
		r_node.mounted(EPOLLIN);
	}
	else
	{
		close(node.fd);
	}
	return;
}
