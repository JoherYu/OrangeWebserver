#include "utils.h"
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
//#include <errno.h>

#include <cstring>

int main(int argc, char *argv[])
{

	int p_fd_r = atoi(argv[1]);
	int p_fd_w = atoi(argv[2]);
	int c_fd_r = atoi(argv[3]);
	int c_fd_w = atoi(argv[4]);
	string path = "";
	if (argc == 7)
	{
		path = argv[6];
	}
	chdir("./components/");
	//char pwd[4096] = {0};
	//getcwd(pwd, 4096);
	//cout << pwd << endl;

	if (strcmp(argv[5], "GET") == 0)
	{
		//cout << "=======" << endl;
		string data = "";
		struct stat st;
		int ret = stat("login.html", &st);
		if (ret == -1)
		{
			perror("stat error:");
			exit(4);
			//data = "error: " + strerror(errno);
		}
		generic_open("login.html", data);
		//cout << data.data() << endl;
		close(p_fd_r);
		//cout << st.st_size << endl;
		ret = write(p_fd_w, data.data(), data.size());
		if (ret < 0)
		{
			//cout << p_fd_w <<endl;
			perror("write error");
		}

		exit(0);
	}
	else if (strcmp(argv[5], "POST") == 0)
	{
		//cout << "login=======" << endl;
		int ret = close(p_fd_w);
		if (ret == -1)
		{
			perror("p_fd_w");
		}
		ret = close(c_fd_r);
		if (ret == -1)
		{
			perror("c_fd_2");
		}
		char buf[1024] = {0};
		//int ret;
		while ((ret = read(p_fd_r, buf, sizeof(buf))) > 0)
		{
			cout << buf << endl;
		};
		//cout << "=========" << endl;
		//char a[4] = "aaa";
		//cout << c_fd_w << endl;
		ret = write(c_fd_w, buf, sizeof(buf));
		//cout << a << endl;
		if (ret < 0)
		{

			perror("write error");
		}
		//cout << c_fd_w << endl;

		exit(0);
	}
}
