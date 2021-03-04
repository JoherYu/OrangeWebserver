#include "http.h"

#include "utils.h"

#include <sys/types.h>
#include <sys/socket.h>

#include <iostream>
#include <memory>
#include <cstring>

using std::cout;

http::http(string protocol, string headers, string data) : protocol(protocol), headers(headers), data(data) {}

void http::add_data(const string &data)
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

char *http::deal_headers(int fd)
{
    int content_length = -1;
    char buf[1024] = {0};
    while (1)
    {
        int len = get_line(fd, buf, sizeof(buf));
        if (len > 0)
        {
            char *key = strtok(buf, ":");
            if (string(key) == "Content-Length")
            {
                content_length = atoi(strtok(NULL, ":"));
            }
            if (buf[0] == '\n')
            {
                break;
            }
            
        }

        else if (len == -1)
        {
            break;
        }
    }

    if (content_length < 0)
    {
        return NULL;
    }

    char *data = new char[content_length];
    int ret = recv(fd, data, content_length, 0);
    return data;
}
