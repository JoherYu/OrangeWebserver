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

	cout << "opening file " << filename << endl;

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

string get_file_type(string& filename)
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

map<string, string> get_conf(const char *filename)
{
	map<string, string> conf;

	ifstream conf_file(filename);
	if (conf_file.fail())
	{
		if (errno == 2)
		{
			cerr << "can't find configure file, load default configuration:" << endl;
		}
		else
		{
			cerr << "can't load configure file, load default configuration:" << endl;
		}
		conf.insert(pair<string, string>("port", "5000"));
		conf.insert(pair<string, string>("max_event_number", "1000"));
		for (auto item : conf)
		{
			cout << item.first << " : " << item.second << endl;
		}
		cout << endl;
	}
	else
	{
		cout << "configure info:" << endl;
		char line[256];
		char *key, *value;
		while (conf_file.getline(line, 256))
		{
			key = strtok(line, "=");
			value = strtok(NULL, "=");
			cout << key << " : " << value << endl;
			conf.insert(pair<string, string>(key, value));
		}
		cout << endl;
	}

	return conf;
}
