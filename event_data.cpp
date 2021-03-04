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

void event_data::error_mounted(int fd, shared_ptr<http_response> response, int error_code, const string &error_descp, const string &error_info)
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
		event_data::error_mounted(node.fd, response, 500, "Server Error", "content recive error");
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
	if (strcmp(method, "GET") == 0)
	{
		string type = get_file_type(str_p.front());
		if (type == "")
		{
			string des_path = "./components/" + str_p.front();
			int ret = stat(des_path.data(), &st);
			if (ret == -1)
			{
				perror("stat error:");
				event_data::error_mounted(node.fd, response, 404, "Not Found", "resouce is missing");
				return;
			}
			else if (st.st_mode & S_IEXEC)
			{
				int p_fd[2];
				ret = pipe(p_fd);
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
					ret = execl(des_path.data(), str_p[0].data(), to_string(p_fd[0]).data(), to_string(p_fd[1]).data(), "-1", "-1", "GET", last_arg, NULL);
					if (ret == -1)
					{
						// todo
						perror("execl error");
					}
				}
				else if (pid > 0)
				{
					close(p_fd[1]);
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
							response = make_shared<http_response>(200, "OK", protocol, str_p[0]);
							while ((ret = read(p_fd[0], buf, sizeof(buf))) > 0)
							{
								response->add_data(buf);
							};
							//cout << response->get_data().size() << endl;
							response->set_content_length(response->get_data().size());
						}
						else if (exit_status == 4)
						{
							//todo: write log WTERMSIG(status));
							event_data::error_mounted(node.fd, response, 404, "Not Found", "resouce is missing");
							return;
						}
						else
						{
							event_data::error_mounted(node.fd, response, 500, "Server Error", "content recive error");
							return;
						}
					}
				}
			}
		}
		else
		{
			int ret = stat(str_p.front().data(), &st);
			if (ret < 0)
			{
				perror("stat error:");
				event_data::error_mounted(node.fd, response, 404, "Not Found", "resouce is missing");
				return;
			}

			response = make_shared<http_response>(200, "OK", protocol, str_p[0], st.st_size);
			try
			{
				open_file(str_p.front().data(), *response);
			}
			catch (const exception &e)
			{
				cerr << "[" << get_time() << "]"
					 << "fail to open file " << str_p[0] << endl;
				cerr << "[" << get_time() << "]" << e.what() << '\n';
				response->set_status_code(500);
				response->set_status_descp("Please Try Again");
			}
		}
	}
	else if (strcmp(method, "POST") == 0)
	{
		int p_fd[2], c_fd[2];
		int ret = pipe(c_fd);
		ret = pipe(p_fd);
		if (ret < 0)
		{
			/* code */
		}
		close(p_fd[0]);
		close(c_fd[1]);
	}

	//struct stat st;
	//int ret = stat(str_p[0].data(), &st);
	//if (ret == -1)
	//{
	//	ret = stat(des_path.data(), &st);
	//	if (ret == -1)
	//	{
	//		perror("stat error:");
	//		event_data::error_mounted(node.fd, response, 404, "Not Found", "resouce is missing");
	//		return;
	//	}
	//}

	/* 	if (S_ISREG(st.st_mode))
	{
		if (st.st_mode & S_IEXEC)
		{ */

	/* 			if (string(method) == "GET")
			{
				c_fd[0] = -1;
				c_fd[1] = -1;
			} */
	//else
	/* 			{
				ret = pipe(c_fd);
				close(p_fd[0]);
				close(c_fd[1]);
				write(p_fd[1], content, strlen(content) + 1);
			} */
	//vector<string> args;
	/* 			for (int arg : p_fd)
			{
				args.push_back(to_string(arg));
			}
			for (int arg : c_fd)
			{
				args.push_back(to_string(arg));
			} */
	/* 			int pid = fork();
			if (pid == 0)
			{
				const char *last_arg = NULL;
				if (str_p.size() != 1)
				{
					last_arg = str_p[1].data();
				}
				ret = execl(des_path.data(), str_p[0].data(), args[0].data(), args[1].data(), args[2].data(), args[3].data(), last_arg, NULL);
				if (ret == -1)
				{
					// todo
					perror("execl error");
				}
			}
			else if (pid > 0)
			{
				close(p_fd[1]);
				int status;
				ret = wait(&status);
				if (ret == -1)
				{
					/*todo code */
	//		}
	//		else
	//		{
	/* todo write log? */
	/*	int exit_status = WEXITSTATUS(status);
			if (exit_status == 0)
			{
				response = make_shared<http_response>(200, "OK", protocol, str_p[0]);
				while ((ret = read(p_fd[0], buf, sizeof(buf))) > 0)
				{
					response->add_data(buf);
				};
				//cout << response->get_data().size() << endl;
				response->set_content_length(response->get_data().size());
			}
			else if (exit_status == 4)
			{
				//todo: write log WTERMSIG(status));
				event_data::error_mounted(node.fd, response, 404, "Not Found", "resouce is missing");
				return;
			}
			else
			{
				event_data::error_mounted(node.fd, response, 500, "Server Error", "content recive error");
				return;
			}
		}
	}
	* /
}
else
{
	/* 	response = make_shared<http_response>(200, "OK", protocol, str_p[0], st.st_size);
	try
	{
		open_file(str_p[0].data(), *response);
	}
	catch (const exception &e)
	{
		cerr << "[" << get_time() << "]"
			 << "fail to open file " << str_p[0] << endl;
		cerr << "[" << get_time() << "]" << e.what() << '\n';
		response->set_status_code(500);
		response->set_status_descp("Please Try Again");
	} */
	//}
	//}
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
	//cout << ret << endl;
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
