#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include "http.h"

using namespace std;

class http_response : public http
{

public:
    http_response(int status_code, string status_descp);
    http_response(int status_code, string status_descp, string protocol, string file_name, long size);
    void set_status_code(int status_code);
    void set_status_descp(string status_descp);
    string get_status_code() override;
    string get_status_descp() override;
    string get_fix_headers() override;

    void set_error_content(string info);
private:
    int status_code;
    string status_descp;
    const string fix_headers = 
        "Server: myserver\r\n";
};

#endif
