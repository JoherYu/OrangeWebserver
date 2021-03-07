#include "event_data.h"

#include "http.h"
#include "http_response.h"
#include "utils.h"
#include "wrappers.h"
#include "global.h"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <cstdio>
#include <iostream>

#include <vector>
#include <array>
#include <exception>

using namespace std;
using namespace std::placeholders;

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
	if (call_back)
	{
		(*call_back)(*this);
	}
	else
	{
		this->unmounted();
	}
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

void event_data::unmounted()
{
	/* 
	if (status != 1)
		return;

	status = 0; */

	Epoll_ctl(epoll_root, EPOLL_CTL_DEL, fd, NULL);
}

int event_data::get_request_line(event_data &node, char *method, char *path, char *protocol)
{
	char buf[1024] = {0};
	int len = 0;
	try
	{
		len = get_line(node.fd, buf, sizeof(buf));
		if (len == 0)
		{
			node.unmounted();
			//close(node.fd);

			return -1;
		}
	}
	catch (const exception &e)
	{
		cerr << "[" << get_time() << "]"
			 << "fail to get_line" << endl;
		cerr << "[" << get_time() << "]" << e.what() << '\n';
		node.unmounted();
		event_exception::error_mounted(node.fd, 5, "content recive error", Static);
		return -1;
	}

	cout << "[" << get_time() << "]" << buf << endl;

	sscanf(buf, "%[^ ] %[^ ] %[^ \n]", method, path, protocol);
	return len;
}

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
		cout << "[" << get_time() << "]" << e.what() << '\n';
	}
	cout << "[" << get_time() << "]"
		 << "new connection from " << inet_ntoa(cin.sin_addr) << ":" << ntohs(cin.sin_port) << endl;
	event_data cnode(cfd, recvdata);
	cnode.mounted(EPOLLIN);
	return;
};

void recvdata(event_data &node)
{
	char method[12], path[1024], protocol[12];
	int len = node.get_request_line(node, method, path, protocol);
	if (len == -1)
		return;

	array<string, 2> str_p = *split_in_2(path + 1, "/");
	str_p.front() = str_p.front() == " " ? "index.html" : str_p.front();

	shared_ptr<http_response> response = make_shared<http_response>(protocol);
	node.unmounted();
	try
	{
		char *content = http::deal_headers(node.fd);

		string type = get_file_type(str_p.front());
		if (type == "")
		{
			string des_path = conf["dynamic_file_dir"] + str_p.front();
			if (Stat(des_path.data()) & S_IEXEC)
				response->deal_dynamic_event(des_path, protocol, node.fd, str_p, content, method);
		}
		else
		{
			Stat(str_p.front().data());
			response->deal_static_event(str_p.front(), protocol);
		}
	}
	catch (const event_exception &e)
	{

		event_exception::error_mounted(node.fd, e.get_error_code(), e.what(), e.get_type());
		return;
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

	cout << "[" << get_time() << "]"
		 << "sending response with status code " << response->get_status_code() << endl;
	int ret = send(node.fd, response_buf.data(), response_buf.size(), 0);
	if (ret < 0)
	{
		perror("sending fail");
	}

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
	return;
}