#ifndef HTTP_H
#define HTTP_H

#include <string>
using namespace std;

class http
{

public:
    http() = default;
    http(string protocol, string headers, string data);
    void add_data(const string& data);
    string get_protocol();
    string get_headers();
    string get_data();

    virtual string get_status_code() = 0;
    virtual string get_status_descp() = 0;
    virtual string get_fix_headers() = 0;
protected:
    string protocol = "HTTP/1.1";
    string headers = "";
    string data = "";
};

inline string http::get_protocol()
{
    return protocol;
}

inline string http::get_headers()
{
    return headers;
}


inline string http::get_data()
{
    return data;
}

#endif
