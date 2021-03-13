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
pthread_mutex_t *event_data::locks = NULL;

event_data::event_data(int fd, void (*call_back)(event_data &)) : fd(fd), call_back(call_back), event_name(0), status(0), len(0),
																  message(NULL)
{
	last_active = time(NULL);
	if (locks == NULL)
	{
		int max_event_number = atoi(conf["max_event_number"].data());
		locks = new pthread_mutex_t[max_event_number];
		for (int i = 0; i < max_event_number; i++)
		{
			pthread_mutex_init(&locks[i], NULL);
		}
	}
}

event_data::event_data(int fd, void (*call_back)(event_data &), shared_ptr<http> message) : fd(fd), call_back(call_back), event_name(0), status(0), len(0),
																							message(message)
{
	last_active = time(NULL);
	if (locks == NULL)
	{
		int max_event_number = atoi(conf["max_event_number"].data());
		locks = new pthread_mutex_t[max_event_number];
		for (int i = 0; i < max_event_number; i++)
		{
			pthread_mutex_init(&locks[i], NULL);
		}
	}
}

void event_data::process()
{
	if (call_back && this->fd > 0)
	{
		(*call_back)(*this);
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
	if (status != 1)
		return;

	status = 0;

	Epoll_ctl(epoll_root, EPOLL_CTL_DEL, fd, NULL);
	delete this;
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
		Fcntl(cfd, F_SETFL, O_NONBLOCK); //设置为非阻塞方式
	}
	catch (const exception &e)
	{
		cout << "[" << get_time() << "]" << e.what() << '\n';
	}
	cout << "[" << get_time() << "]"
		 << "new connection from " << inet_ntoa(cin.sin_addr) << ":" << ntohs(cin.sin_port) << endl;

	node.unmounted();
	event_data *cnode = new event_data(cfd, recvdata);
	cnode->mounted(EPOLLIN);
};

void recvdata(event_data &node)
{
	pthread_mutex_lock(&node.locks[node.fd]);
	char method[12], path[1024], protocol[12];
	int len = node.get_request_line(node, method, path, protocol);
	if (len == -1)
		return;

	array<string, 2> str_p = *split_in_2(path + 1, "/");
	str_p.front() = str_p.front() == " " ? "index.html" : str_p.front();

	shared_ptr<http_response> response = make_shared<http_response>(protocol);

	int fd = node.fd;
	try
	{
		char *content = http::deal_headers(fd);
		pthread_mutex_unlock(&node.locks[node.fd]);
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
		node.unmounted();
		event_exception::error_mounted(fd, e.get_error_code(), e.what(), e.get_type());
		return;
	}

	if (len > 0)
	{
		node.unmounted();
		event_data *wnode = new event_data(fd, senddata, response);
		wnode->mounted(EPOLLOUT);
	}
	else
	{
		close(fd);
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

	int fd = node.fd;

	if (ret > 0)
	{
		node.unmounted();
		event_data *rnode = new event_data(fd, recvdata);
		rnode->mounted(EPOLLIN);
	}
	else
	{
		close(fd);
	}
	return;
}