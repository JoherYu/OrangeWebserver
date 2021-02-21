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
#include <iostream>
#include <fstream>

using namespace std;

void open_file(const char *filename, http &response)
{
	int fd = Open(filename, O_RDONLY);
	char buf[4096] = {0};
	int len = 0;

	cout << "[" << get_time() << "]"
		 << "opening file " << filename << endl;

	while ((len = Read(fd, buf, sizeof(buf))) > 0)
	{
		response.add_data(string(buf, sizeof(buf)));
	}
	close(fd);

	//return 0; //todo: sucess flag
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

string get_file_type(string &filename)
{
	string str_suffix = filename.substr(filename.find_last_of('.') + 1);
	if (str_suffix == "jpg")
	{
		return "image/jpeg";
	}
	else if (str_suffix == ".ico")
	{
		return "image/x-icon";
	}
}

void load_default_conf(map<string, string> &configuration)
{
	configuration.insert(pair<string, string>("port", "5000"));
	configuration.insert(pair<string, string>("max_event_number", "1000"));
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
		char *key, *value;
		while (conf_file.getline(line, 256))
		{
			key = strtok(line, "=");
			value = strtok(NULL, "=");
			if (key == 0)
			{
				continue;
			}

			cout << "[" << get_time() << "]" << key << " : " << value << endl;
			conf.insert(pair<string, string>(key, value));
		}
		cout << "[" << get_time() << "]" << endl;
	}

	cout << "[" << get_time() << "]"
		 << "set static file dir" << endl;
	auto path_iter = conf.find("static_file_dir");
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

		conf.insert(pair<string, string>("static_file_dir", "./"));
		cout << "[" << get_time() << "]"
			 << "change static_file_dir to work dir" << endl;
	}
	else
	{
		const char *path = (path_iter->second).data();
		struct stat st;
		int ret = stat(path, &st);
		if (ret == -1 || !S_ISDIR(st.st_mode))
		{
			perror("stat error:");
			cout << "[" << get_time() << "]"
				 << "item: static_file_dir error, change to work dir" << endl;
			conf["static_file_dir"] = "./";
		}
		else
		{
			cout << "[" << get_time() << "]"
				 << "static file dir at " << path << "\n"
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
