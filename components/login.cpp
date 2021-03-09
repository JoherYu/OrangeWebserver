#include "utils.h"
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <memory>
#include <map>
//#include <errno.h>

#include <cstring>

#include "rapidjson/document.h"
#include <mysql/mysql.h>

using namespace rapidjson;
using namespace std;

shared_ptr<map<string, string>> parse_json(string req_str);
string sql_query(shared_ptr<map<string, string>> user);

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

	if (strcmp(argv[5], "GET") == 0)
	{
		string data = "";
		struct stat st;
		int ret = stat("login.html", &st);
		if (ret == -1)
		{
			perror("stat error");
			exit(4);
		}
		generic_open("login.html", data);
		close(p_fd_r);
		ret = write(p_fd_w, data.data(), data.size());
		if (ret < 0)
		{
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
		string req_str;

		while ((ret = read(p_fd_r, buf, sizeof(buf))) > 0)
		{
			req_str += buf;
		};

		shared_ptr<map<string, string>> user_info = parse_json(req_str);
		string res_str = sql_query(user_info);

		ret = write(c_fd_w, res_str.data(), res_str.size());
		if (ret < 0)
		{

			perror("write error");
		}
		exit(0);
	}
}

shared_ptr<map<string, string>> parse_json(string req_str)
{
	shared_ptr<map<string, string>> result = make_shared<map<string, string>>();
	Document document;
	document.Parse(req_str.data());
	result->insert(pair<string, string>("username", document["username"].GetString()));
	result->insert(pair<string, string>("password", document["password"].GetString()));
	return result;
}
string sql_query(shared_ptr<map<string, string>> user)
{

	string res_str; // didn't use for now
	string query = "select * from users where username = " + (*user)["username"];

	MYSQL conn;
	mysql_init(&conn);
	if (!mysql_real_connect(&conn, "localhost", "root", "123456", "test", 0, NULL, CLIENT_FOUND_ROWS))
	{
		cout << mysql_error(&conn) << endl;
		exit(5);
	}

	cout << query << endl;
	MYSQL_RES *result;
	MYSQL_ROW row;
	if ((mysql_query(&conn, query.data()) == 0) &&
		(result = mysql_store_result(&conn)) &&
		(row = mysql_fetch_row(result)))
	{
		if (strcmp(row[1], (*user)["password"].data()) != 0)
			exit(43);
	}
	else
	{
		cout << mysql_error(&conn) << endl;
		exit(5);
	}
	mysql_close(&conn);

	return res_str;
}
