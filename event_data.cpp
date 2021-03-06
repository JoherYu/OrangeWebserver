#include "event_data.h"

#include "http.h"
#include "http_response.h"
#include "utils.h"
#include "wrappers.h"

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
		(*call_back)(*this); /* find solution*/
	}
	else
	{
		this->unmounted();
		//close?
	}
}
void event_data::deal_static_resource(shared_ptr<http_response> response, string filename)
{

	try
	{
		open_file(filename.data(), *response);
	}
	catch (const exception &e)
	{
		cerr << "[" << get_time() << "]"
			 << "fail to open file " << filename << endl;
		cerr << "[" << get_time() << "]" << e.what() << '\n';
		response->set_status_code(500);
		response->set_status_descp("Please Try Again");
	}
}

void event_data::deal_dynamic_resource(shared_ptr<http_response> response, int r_fd)
{
	char buf[1024] = {0};
	int ret = 0;
	while ((ret = read(r_fd, buf, sizeof(buf))) > 0)
	{
		response->add_data(buf);
	};
}

void event_data::make_response(shared_ptr<http_response> &response, string filename, string protocol, function<void(shared_ptr<http_response>)> deal_func)
{
	response = make_shared<http_response>(200, "OK", protocol);
	deal_func(response);
	//order matters
	response->set_content_type(get_file_type(filename));
	response->set_content_length(response->get_data().size());
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

void event_data::error_mounted(int fd, int error_code, const string &error_data, response_type type)
{
	string error_descp;
	int status_code;
	string content_type = (type == Static) ? get_file_type(".html") : get_file_type(".json");
	if (error_code == 4)
	{
		status_code = 404;
		error_descp = "Not Found";
	}
	else if (error_code == 43)
	{
		status_code = 403;
		error_descp = "Forbidden";
	}
	else if (error_code == 5)
	{
		status_code = 500;
		error_descp = "Server Error";
	}

	shared_ptr<http_response> response = make_shared<http_response>(status_code, error_descp);
	response->set_content_type(content_type);
	response->set_error_content(error_data, type);
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

int event_data::deal_dynamic_event(shared_ptr<http_response> &response, string des_path, char *protocol, int fd, array<string, 2> &str_p, char *content, char *method)
{
	int ret = 0;
	int p_fd[2], c_fd[2];
	ret = pipe(p_fd);
	if (strcmp(method, "GET") == 0)
	{
		c_fd[0] = -1;
		c_fd[1] = -1;
	}
	else
	{
		ret = pipe(c_fd);
	}

	if (ret < 0)
	{
		/* code */
	}
	int pid = fork();
	if (pid == 0)
	{

		const char *last_arg = NULL;
		if (str_p.back() != "")
		{
			last_arg = str_p.back().data();
		}
		ret = execl(des_path.data(), str_p.front().data(), to_string(p_fd[0]).data(), to_string(p_fd[1]).data(), to_string(c_fd[0]).data(), to_string(c_fd[1]).data(), method, last_arg, NULL);
		if (ret == -1)
		{
			// todo
			perror("execl error");
		}
	}
	else if (pid > 0)
	{
		if (strcmp(method, "GET") == 0)
		{
			close(p_fd[1]);
		}
		else
		{

			ret = close(p_fd[0]);
			if (ret == -1)
			{
				perror("p_fd_0");
			}

			ret = close(c_fd[1]);
			if (ret == -1)
			{
				perror("c_fd_1");
			}
			write(p_fd[1], content, string(content).size());
			close(p_fd[1]);
		}

		int status;
		ret = wait(&status);
		if (ret == -1)
		{
			/*todo code */
		}
		else
		{
			/* todo write log? */
			int exit_status = WEXITSTATUS(status);
			if (exit_status == 0)
			{
				int r_fd = strcmp(method, "POST") == 0 ? c_fd[0] : p_fd[0];
				make_response(response, str_p.front(), protocol, bind(deal_dynamic_resource, _1, r_fd));
				return 0;
			}
			else
			{
				event_data::error_mounted(fd, exit_status, "content recive error", Dynamic);
				return -1;
			}
		}
	}
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
	shared_ptr<http_response> response;
	char buf[1024] = {0};
	int len = 0;
	try
	{
		len = get_line(node.fd, buf, sizeof(buf));
		if (len == 0)
		{
			node.unmounted();
			//close(node.fd);

			return;
		}
	}
	catch (const exception &e)
	{
		cerr << "[" << get_time() << "]"
			 << "fail to get_line" << endl;
		cerr << "[" << get_time() << "]" << e.what() << '\n';
		node.unmounted();
		event_data::error_mounted(node.fd, 5, "content recive error", Static);
		return;
	}

	cout << "[" << get_time() << "]" << buf << endl;

	char method[12], path[1024], protocol[12];
	sscanf(buf, "%[^ ] %[^ ] %[^ \n]", method, path, protocol);
	node.unmounted();

	char *content = http::deal_headers(node.fd);

	array<string, 2> str_p = *split_in_2(path + 1, "/");
	if (str_p.front() == " ")
	{
		str_p.front() = "index.html";
	};
	;
	struct stat st;

	string type = get_file_type(str_p.front());
	if (type == "")
	{
		string des_path = "./components/" + str_p.front();
		int ret = stat(des_path.data(), &st);
		if (ret == -1)
		{
			perror("stat error:");
			event_data::error_mounted(node.fd, 4, "resouce is missing", Dynamic);
			return;
		}
		else if (st.st_mode & S_IEXEC)
		{
			int ret = event_data::deal_dynamic_event(response, des_path, protocol, node.fd, str_p, content, method);
			if (ret == -1)
				return;
		}
	}
	else
	{
		int ret = stat(str_p.front().data(), &st);
		if (ret < 0)
		{
			perror("stat error:");
			event_data::error_mounted(node.fd, 4, "resouce is missing", Static);
			return;
		}
		event_data::make_response(response, str_p.front(), protocol, bind(event_data::deal_static_resource, _1, str_p.front()));
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
	//delete &node;
	return;
}