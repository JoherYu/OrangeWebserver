#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include "http.h"

using namespace std;

class http_request : public http
{

public:
    http_request(string method, string url, string protocol);

private:
    string method;
    string url;
};

#endif
