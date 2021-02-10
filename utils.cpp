#include "utils.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include "http.h"

using namespace std;

int open_file(const char *filename, http &response)
{
	int fd = open(filename, O_RDONLY);
	char buf[4096] = {0};
	int len = 0;
	while ((len = read(fd, buf, sizeof(buf))) > 0)
	{
		string a(buf, sizeof(buf));
		response.add_data(a);
	}
	close(fd);

	return 0; //todo: error/sucess code
}

int get_line(int fd, char *buf, int buf_size)
{
	char c = '\0';
	int i = 0;
	int n = 0;
	while ((i < buf_size - 1) && (c != '\n'))
	{
		n = recv(fd, &c, 1, 0);
		if (n > 0)
		{
			if (c == '\r')
			{
				n = recv(fd, &c, 1, MSG_PEEK);
				if ((n > 0) && (c == '\n'))
				{
					recv(fd, &c, 1, 0);
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
	else if (str_suffix == ".ico")
	{
		return "image/x-icon";
	}
}
