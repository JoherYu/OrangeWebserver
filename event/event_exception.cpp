#include "event_exception.h"
#include "http_response.h"
#include "event_data.h"
#include "utils.h"
#include <memory>

event_exception::event_exception(string message) : message("event_data error: " + message){};
event_exception::event_exception(int error_code, string message, response_type type) : error_code(error_code), message(message), type(type){};
void event_exception::error_mounted(int fd, int error_code, const string &error_data, response_type type)
{
    string error_descp;
    int status_code;
    string content_type = (type == Static) ? get_file_type(".html") : get_file_type(".json");
    http_response::set_error_info(error_code, status_code, error_descp);

    shared_ptr<http_response> response = make_shared<http_response>(status_code, error_descp);
    response->set_content_type(content_type);
    response->set_error_content(error_data, type);
    event_data *w_node = new event_data(fd, senddata, response);
    w_node->mounted(EPOLLOUT);
}

void throw_exception(int ret_value)
{
	throw event_exception(strerror(errno));
}