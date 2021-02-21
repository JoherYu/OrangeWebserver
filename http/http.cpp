#include "http.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>

using std::cout;

http::http(string protocol, string headers, string data) : protocol(protocol), headers(headers), data(data) {}

void http::add_data(const string& data)
{
    if (this->data == "")
    {
        this->data = data;
    }
    else
    {
        this->data.append(data);
    }
}


