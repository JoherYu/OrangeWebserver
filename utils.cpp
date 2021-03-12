#include "utils.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "http.h"
#include "wrappers.h"

#include <cstdio>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <iostream>
#include <fstream>

using namespace std;

void open_file(const char *filename, http &response)
{
	string data = "";
	generic_open(filename, data);
	response.add_data(data);
}

void generic_open(const char *filename, string &data)
{
	int fd = Open(filename, O_RDONLY);
	char buf[4096] = {0};
	int len = 0;

	cout << "[" << get_time() << "]"
		 << "opening file " << filename << endl;

	while ((len = Read(fd, buf, sizeof(buf))) > 0)
	{
		data += string(buf, sizeof(buf));
	}
	close(fd);
}
/* void create_pipes(initializer_list<int*> arrs){

	for (auto curr = arrs.begin(); curr != arrs.end(); curr++)  
	{
		Pipe(*curr);
	}
} */
void create_dup_pipe(int *const p_fd, int *const c_fd, const char *const method)
{
	Pipe(p_fd);
	if (strcmp(method, "GET") == 0)
	{
		c_fd[0] = -1;
		c_fd[1] = -1;
	}
	else
	{
		Pipe(c_fd);
	}
}

void p_deal_pipe(int *p_fd, int *c_fd, char *method, char *content)
{
	if (strcmp(method, "GET") == 0)
	{
		Close(p_fd[1], "p_fd[1]");
	}
	else
	{

		Close(p_fd[0], "p_fd[0]");
		Close(c_fd[1], "c_fd[1]");
		Write(p_fd[1], content, "p_fd[1]");
		Close(p_fd[1], "p_fd[1]");
	}
}
int get_line(int fd, char *buf, int buf_size)
{
	char c = '\0';
	int i = 0;
	int n = 0;
	while ((i < buf_size - 1) && (c != '\n'))
	{
		n = Recv(fd, &c, 1, 0);
		if (n > 0)
		{
			if (c == '\0')
			{
				continue;
			}

			if (c == '\r')
			{
				n = Recv(fd, &c, 1, MSG_PEEK);
				if ((n > 0) && (c == '\n'))
				{
					Recv(fd, &c, 1, 0);
				}
				else
				{
					c = '\n';
				}
			}
			buf[i] = c;
			i++;
		}
		else
		{
			c = '\n';
		}
	}
	buf[i] = '\0';

	return i;
}

string get_file_type(string filename)
{
	string str_suffix = filename.substr(filename.find_last_of('.') + 1);
	if (str_suffix == "jpg")
	{
		return "image/jpeg";
	}
	else if (str_suffix == "ico")
	{
		return "image/x-icon";
	}
	else if (str_suffix == "html")
	{
		return "text/html";
	}
	else
	{
		return "";
	}
}

void load_default_conf(map<string, string> &configuration)
{
	configuration.insert(pair<string, string>("port", "5000"));
	configuration.insert(pair<string, string>("max_event_number", "1000"));
	configuration.insert(pair<string, string>("adjust_thread_interval", "10"));
	configuration.insert(pair<string, string>("min_wait_task_num", "10"));
	configuration.insert(pair<string, string>("thread_vary_num", "10"));
}

void get_conf(const char *filename, map<string, string> &conf)
{
	ifstream conf_file(filename);
	if (conf_file.fail())
	{
		if (errno == 2)
		{
			cerr << "[" << get_time() << "]"
				 << "can't find configure file, load default configuration:" << endl;
		}
		else
		{
			cerr << "[" << get_time() << "]"
				 << "can't load configure file, load default configuration:" << endl;
		}
		load_default_conf(conf);
		for (auto item : conf)
		{
			cout << "[" << get_time() << "]" << item.first << " : " << item.second << endl;
		}
		cout << "[" << get_time() << "]" << endl;
	}
	else
	{
		cout << "[" << get_time() << "]"
			 << "configure info:" << endl;
		char line[256];
		string key, value;
		while (conf_file.getline(line, 256))
		{
			array<string, 2> conf_pair = *split_in_2(line, "=");
			key = conf_pair.front();
			value = conf_pair.back();
			if (key == " ")
			{
				continue;
			}

			cout << "[" << get_time() << "]" << key << " : " << value << endl;
			conf.insert(pair<string, string>(key, value));
		}
		cout << "[" << get_time() << "]" << endl;
	}

	check_work_dir(conf, "static_file_dir", "./");
	check_work_dir(conf, "dynamic_file_dir", "./components/");
}

void check_work_dir(map<string, string> &conf, string dir, string default_val)
{
	cout << "[" << get_time() << "]"
		 << "set " << dir << endl;
	auto path_iter = conf.find(dir);
	if (path_iter == conf.end())
	{
		/*
	    char curr_path[256];
	    char *ret = getcwd(curr_path, 256);
        if (ret == NULL)
	    {
		    cerr << "work directory couldn't be determined or pathname was too large.";
            exit(-1);
	    }
		*/
		conf.insert(pair<string, string>(dir, default_val));
		cout << "[" << get_time() << "]"
			 << "change " << dir << " to work dir" << endl;
	}
	else
	{
		const char *path = (path_iter->second).data();
		struct stat st;
		int ret = stat(path, &st);
		if (ret == -1 || !S_ISDIR(st.st_mode))
		{
			perror("stat error");
			cout << "[" << get_time() << "]"
				 << "item: " << dir << " error, change to default work dir" << endl;
			// todo : create default
			conf[dir] = default_val;
		}
		else
		{
			cout << "[" << get_time() << "]"
				 << dir << " at " << path << "\n"
				 << endl;
		}
	}
}

char *get_time()
{
	time_t now = time(0);
	char *dt = ctime(&now);
	dt[strlen(dt) - 1] = 0;
	return dt;
}

shared_ptr<array<string, 2>> split_in_2(char *s, const char *delim)
{
	shared_ptr<array<string, 2>> result = make_shared<array<string, 2>>();
	if (*s == '\0' || *delim == '\0')
	{
		result->front() = " ";
		return result;
	}

	char *p = strtok(s, delim);
	result->front() = p;
	p = strtok(NULL, delim);
	if (p)
	{
		result->back() = p;
	}

	return result;
}

/* shared_ptr<vector<string>> split_path(char *s, const char *delim)
{
	shared_ptr<vector<string>> result = make_shared<vector<string>>();
	if (*s == '\0')
	{
		result->push_back(" ");
		return result;
	}

	char *p = strtok(s, delim);
	result->push_back(p);
	p = strtok(NULL, delim);
	if (p)
	{
		result->push_back(p);
	}

	return result;
} */

/* shared_ptr<vector<string>> split_string(char *s, const char *delim)
{
	shared_ptr<vector<string>> result = make_shared<vector<string>>();
	char *p = strtok(s, delim);
	while (p)
	{
		result->push_back(p);
		p = strtok(NULL, delim);
	}
	return result;
} */