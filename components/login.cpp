#include "utils.h"
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
//#include <errno.h>

#include <cstring>

#include "rapidjson/document.h"
//#include "rapidjson/writer.h"
//#include "rapidjson/stringbuffer.h"

#include <mysql/mysql.h>

using namespace rapidjson;
using namespace std;

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
		string json_str;
		string res_str;
		while ((ret = read(p_fd_r, buf, sizeof(buf))) > 0)
		{
			json_str += buf;
		};

		Document document;
		document.Parse(json_str.data());
		for (Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
		{
			cout << itr->name.GetString() << " = " << itr->value.GetString() << endl;
		}
		char query[38] = "select * from users where username = ";
		strcat(query, document["username"].GetString());
		MYSQL conn;
		int res;
		mysql_init(&conn);
		if (mysql_real_connect(&conn, "localhost", "root", "123456", "test", 0, NULL, CLIENT_FOUND_ROWS)) //"root":数据库管理员 "":root密码 "test":数据库的名字
		{
			//printf("connect success!\n");
			cout << query << endl;
			res = mysql_query(&conn, query);
			//cout << res << endl;
			if (!res)
			{
				MYSQL_RES *result = mysql_store_result(&conn);
				if (result)
				{
					cout << "========" << endl;
					MYSQL_ROW row = mysql_fetch_row(result);
					if (row)
					{
						cout << strcmp(row[1], document["password"].GetString()) << endl;
						//res_str += "<html><head><title>Login Successful</title></head>\n<body bgcolor=\"#cc99cc\"><h2 align=\"center\">Login Successful</h2>\n\n<hr>\n</body>\n</html>\n";
					    //res_str = "{}";
					}
				}

				printf("error\n");
			}
			else
			{
				//printf("OK\n");
			}

			mysql_close(&conn);
		}
		else
		{
			cout << mysql_error(&conn) << endl;
		}

		ret = write(c_fd_w, res_str.data(), res_str.size());
		if (ret < 0)
		{

			perror("write error");
		}
		//cout << c_fd_w << endl;

		exit(0);
	}
}
