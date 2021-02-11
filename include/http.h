#ifndef HTTP_H
#define HTTP_H

#include <string>
using namespace std;

class http
{

public:
    http(string protocol, string headers, string data);
    void add_data(string data);
    string get_protocol();
    string get_headers();
    string get_data();

    virtual string get_status_code() = 0;
    virtual string get_status_descp() = 0;

protected:
    string protocol;
    string headers;
    string data;
};

#endif
