#include "utils.h"
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
//#include <errno.h>

#include <cstring>


int main(int argc, char *argv[])
{
	//cout << "login=======" << endl;
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
	else
	{
		//todo:connect database
	}
}
