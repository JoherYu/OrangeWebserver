#include "event_exception.h"

event_exception::event_exception(string message) : message("event_data error: " + message){};
event_exception::event_exception(int fd, int error_code, string message, response_type type) : fd(fd), error_code(error_code), message("event_data error: " + message), type(type){};
