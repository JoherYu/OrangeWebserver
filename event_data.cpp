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
#include "wrappers.h"
#include <exception>

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

	Epoll_ctl(epoll_root, op, fd, &epv);
}

void event_data::error_mounted(int fd, shared_ptr<http_response> response, int error_code, string error_descp, string error_info)
{
	response = make_shared<http_response>(error_code, error_descp);
	response->set_error_content(error_info);
	event_data *w_node = new event_data(fd, senddata, response);
	w_node->mounted(EPOLLOUT);
}

void event_data::unmounted()
{

	if (status != 1)
		return;

	status = 0;
	Epoll_ctl(epoll_root, EPOLL_CTL_DEL, fd, NULL);
}

void event_data::set_root(int root_fd)
{
	epoll_root = root_fd;
};

void acceptconn(event_data &node)
{
	struct sockaddr_in cin;
	socklen_t len = sizeof(cin);
	int cfd = Accept(node.fd, (struct sockaddr *)&cin, &len);
	try
	{
		Fcntl(cfd, F_SETFL, O_NONBLOCK);
	}
	catch (const exception &e)
	{
		cout << e.what() << '\n';
	}

	event_data cnode(cfd, recvdata);
	cnode.mounted(EPOLLIN);
	return;
};

void recvdata(event_data &node)
{
	shared_ptr<http_response> response;
	char buf[1024] = {0};
	int len = 0;
	try
	{
		len = get_line(node.fd, buf, sizeof(buf));
	}
	catch (const exception &e)
	{
		cerr << e.what() << '\n';
		event_data::error_mounted(node.fd, response, 500, "Server Error", "content recive error");
		return;
	}

	char method[12], path[1024], protocol[12];
	sscanf(buf, "%[^ ] %[^ ] %[^ \n]", method, path, protocol);

	int ret = recv(node.fd, buf, sizeof(buf), 0);
	if (ret == -1)
	{
		cerr << "empty buf fail" << endl;
	}

#if 0	
	http_request request(method, url, protocol);
    if(method == "POST"){
		buf = {0};	
	    recv(node.fd, buf, sizeof(buf), 0);
		// todo extract data
	}
#endif

	node.unmounted();

	char *file_path = path + 1;
	struct stat st;
	auto file = file_path[0] == '\0' ? "index.html" : file_path;
	ret = stat(file, &st);
	if (ret == -1)
	{
		perror("stat error:");
		event_data::error_mounted(node.fd, response, 404, "Not Found", "resouce is missing");
		return;
	}

	if (S_ISDIR(st.st_mode))
	{
		//todo: return directory
	}
	else if (S_ISREG(st.st_mode))
	{
		response = make_shared<http_response>(200, "OK", protocol, file, st.st_size);
		try
		{
			open_file(file, *response);
		}
		catch (const exception &e)
		{
			cerr << e.what() << '\n';
			response->set_status_code(500);
			response->set_status_descp("Please Try Again");
		}
	}

	if (len > 0)
	{
		event_data *w_node = new event_data(node.fd, senddata, response);
		w_node->mounted(EPOLLOUT);
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
		response->get_protocol() + " " + response->get_status_code() + " " + response->get_headers() + response->get_fix_headers() +
		"\r\n" +
		response->get_data();

	int ret = send(node.fd, response_buf.data(), response_buf.size(), 0);

	node.unmounted();

	if (ret > 0)
	{
		event_data r_node = event_data(node.fd, recvdata);
		r_node.mounted(EPOLLIN);
	}
	else
	{
		close(node.fd);
	}
	delete &node;
	return;
}
