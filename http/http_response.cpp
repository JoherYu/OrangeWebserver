#include "http_response.h"
#include "utils.h"

http_response::http_response(int status_code, string status_descp,
                             string protocol, string file_name, long size) : status_code(status_code),
                                                                             status_descp(status_descp), http(protocol, fix_headers, "")
{
    this->headers.append("Content-Type:").append(get_file_type(file_name)).append("\r\n");
    this->headers.append("Content-Length:").append(to_string(size)).append("\r\n");
}

void http_response::set_status_code(int status_code)
{
    this->status_code = status_code;
}

void http_response::set_status_descp(string status_descp)
{
    this->status_descp = status_descp;
}
string http_response::get_status_code()
{
    return to_string(status_code);
}

string http_response::get_status_descp()
{
    return status_descp;
}
