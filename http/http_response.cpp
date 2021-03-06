#include "http_response.h"
#include "utils.h"

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