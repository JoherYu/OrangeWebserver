#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <array>
#include "http.h"

#include <functional>

using namespace std;

class http_response : public http
{

public:
    explicit http_response(string protocol);
    http_response(int status_code, string status_descp);

    http_response(int status_code, string status_descp, string protocol);
    void set_content_type(string type_name);
    void set_content_length(long size);
    void set_status_code(int status_code);
    void set_status_descp(const string &status_descp);
    string get_status_code() override;
    string get_status_descp() override;
    string get_fix_headers() override;

    void set_error_content(const string &info, response_type type);

    void deal_static_resource(string filename);
    void deal_dynamic_resource(int r_fd);
    void make_response(string filename, string protocol, function<void()> deal_func);
    void deal_dynamic_event(string des_path, char *protocol, int fd, array<string, 2> &str_p, char *content, char *method);
    void deal_static_event(string filepath, char *protocol);

    static void set_error_info(const int error_code, int &status_code, string &error_descp);

private:
    int status_code;
    string status_descp;
    const string fix_headers =
        "Server: myserver\r\n";
};

inline void http_response::set_status_code(int status_code)
{
    this->status_code = status_code;
}

inline void http_response::set_status_descp(const string &status_descp)
{
    this->status_descp = status_descp;
}
inline string http_response::get_status_code()
{
    return to_string(status_code);
}

inline string http_response::get_status_descp()
{
    return status_descp;
}
inline string http_response::get_fix_headers()
{
    return fix_headers;
}

#endif
