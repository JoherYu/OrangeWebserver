#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include "http.h"


using namespace std;

class http_response : public http
{

public:
    http_response(int status_code, string status_descp);

    http_response(int status_code, string status_descp, string protocol);
    void set_content_type(string type_name);
    void set_content_length(long size);
    void set_status_code(int status_code);
    void set_status_descp(const string& status_descp);
    string get_status_code() override;
    string get_status_descp() override;
    string get_fix_headers() override;

    void set_error_content(const string& info, response_type type);
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

inline void http_response::set_status_descp(const string& status_descp)
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
