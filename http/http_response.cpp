#include "http_response.h"
#include "utils.h"
#include "wrappers.h"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <cstring>
#include "event_data.h"

using namespace std;
using namespace std::placeholders;

http_response::http_response(string protocol) : http(protocol, "", "") {}
http_response::http_response(int status_code, string status_descp) : status_code(status_code), status_descp(status_descp), http() {}

http_response::http_response(int status_code, string status_descp,
                             string protocol) : status_code(status_code),
                                                status_descp(status_descp), http(protocol, "", "")
{
}

void http_response::set_content_type(string type_name)
{
    this->headers.append("Content-Type:").append(type_name).append("\r\n");
}
void http_response::set_content_length(long size)
{
    if (size == -1)
    {
        size = data.size();
    }

    this->headers.append("Content-Length:").append(to_string(size)).append("\r\n");
}
void http_response::set_error_content(const string &error_info, response_type type)
{
    data = (type == Dynamic) ? error_info :

                             "<html><head><title>" + to_string(status_code) + " " + status_descp + "</title></head>\n" +
                                 "<body bgcolor=\"#cc99cc\"><h2 align=\"center\">" + to_string(status_code) + " " + status_descp + "</h2>\n" +
                                 error_info + "\n" +
                                 "<hr>\n</body>\n</html>\n";

    headers = "Content-Type:text/html\r\nContent-Length:" + to_string(data.size()) + "\r\n";
}
void http_response::deal_static_resource(string filename)
{

    try
    {
        open_file(filename.data(), *this);
    }
    catch (const exception &e)
    {
        cerr << "[" << get_time() << "]"
             << "fail to open file " << filename << endl;
        cerr << "[" << get_time() << "]" << e.what() << '\n';
        perror("static open error");
        throw_exception(5, "server error", Dynamic);
    }
}

void http_response::deal_dynamic_resource(int r_fd)
{
    char buf[1024] = {0};
    int ret = 0;
    while ((ret = read(r_fd, buf, sizeof(buf))) > 0)
    {
        this->add_data(buf);
    };

    if (ret < 0)
    {
        perror("dynamic read error");
        throw_exception(5, "server error", Dynamic);
    }
}

void http_response::make_response(string filename, string protocol, function<void()> deal_func)
{

    deal_func();
    this->set_status_code(200);
    this->set_status_descp("OK");
    //order matters
    this->set_content_type(get_file_type(filename));
    this->set_content_length(this->get_data().size());
}

void http_response::deal_dynamic_event(string des_path, char *protocol, int fd, array<string, 2> &str_p, char *content, char *method)
{
    int p_fd[2], c_fd[2];
    create_dup_pipe(p_fd, c_fd, method);

    int pid = fork();
    if (pid == 0)
    {
        const char *last_arg = str_p.back() != "" ? str_p.back().data() : NULL;
        int ret = execl(des_path.data(), str_p.front().data(), to_string(p_fd[0]).data(), to_string(p_fd[1]).data(), to_string(c_fd[0]).data(), to_string(c_fd[1]).data(), method, last_arg, NULL);
        if (ret == -1)
        {
            perror("execl error");
            throw_exception(5, "server error", Dynamic);
        }
    }
    else if (pid > 0)
    {
        p_deal_pipe(p_fd, c_fd, method, content);

        int exit_status = Wait();
        if (exit_status != 0)
            throw_exception(exit_status, "server error", Dynamic);

        int r_fd = strcmp(method, "POST") == 0 ? c_fd[0] : p_fd[0];
        make_response(str_p.front(), protocol, bind(&http_response::deal_dynamic_resource, this, r_fd));
    }
}

void http_response::deal_static_event(string filepath, char *protocol)
{
    make_response(filepath, protocol, bind(&http_response::deal_static_resource, this, filepath));
}

void http_response::set_error_info(const int error_code, int &status_code, string &error_descp)
{
    if (error_code == 4)
    {
        status_code = 404;
        error_descp = "Not Found";
    }
    else if (error_code == 43) //todo enum
    {
        status_code = 403;
        error_descp = "Forbidden";
    }
    else if (error_code == 5)
    {
        status_code = 500;
        error_descp = "Server Error";
    }
}