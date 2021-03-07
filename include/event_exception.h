#ifndef EVENT_EXCEPTION_H
#define EVENT_EXCEPTION_H

#include <stdexcept>
#include <string>
#include "http.h"

using namespace std;

class event_exception : public exception
{

public:
	event_exception(int fd, int error_code, string message, response_type type);
	explicit event_exception(string message);
	virtual const char *what() const throw()
	{
		return message.data();
	};

private:
	int fd;
	int error_code;
	string message;
	response_type type;
	//string func_name;
};

#endif
