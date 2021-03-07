#include "http_response.h"
#include "utils.h"
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
        this->set_status_code(500);
        this->set_status_descp("Please Try Again");
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
}

void http_response::make_response(string filename, string protocol, function<void()> deal_func)
{
    this->set_status_code(200);
    this->set_status_descp("OK");
    deal_func();
    //order matters
    this->set_content_type(get_file_type(filename));
    this->set_content_length(this->get_data().size());
}

int http_response::deal_dynamic_event(string des_path, char *protocol, int fd, array<string, 2> &str_p, char *content, char *method)
{
    int ret = 0;
    int p_fd[2], c_fd[2];
    ret = pipe(p_fd);
    if (strcmp(method, "GET") == 0)
    {
        c_fd[0] = -1;
        c_fd[1] = -1;
    }
    else
    {
        ret = pipe(c_fd);
    }

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
        ret = execl(des_path.data(), str_p.front().data(), to_string(p_fd[0]).data(), to_string(p_fd[1]).data(), to_string(c_fd[0]).data(), to_string(c_fd[1]).data(), method, last_arg, NULL);
        if (ret == -1)
        {
            // todo
            perror("execl error");
        }
    }
    else if (pid > 0)
    {
        if (strcmp(method, "GET") == 0)
        {
            close(p_fd[1]);
        }
        else
        {

            ret = close(p_fd[0]);
            if (ret == -1)
            {
                perror("p_fd_0");
            }

            ret = close(c_fd[1]);
            if (ret == -1)
            {
                perror("c_fd_1");
            }
            write(p_fd[1], content, string(content).size());
            close(p_fd[1]);
        }

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
                int r_fd = strcmp(method, "POST") == 0 ? c_fd[0] : p_fd[0];
                make_response(str_p.front(), protocol, bind(&http_response::deal_dynamic_resource, this, r_fd));
                return 0;
            }
            else
            {
                event_data::error_mounted(fd, exit_status, "content recive error", Dynamic);
                return -1;
            }
        }
    }
}

int http_response::deal_static_event(string filepath, char *protocol){
    make_response(filepath, protocol, bind(&http_response::deal_static_resource, this, filepath));
    return 0;
}