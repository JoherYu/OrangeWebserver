#ifndef EVENT_EXCEPTION_H
#define EVENT_EXCEPTION_H

#include <stdexcept>
#include <string>
#include <cstring>
#include "http.h"

using namespace std;

class event_exception : public exception
{

public:
	event_exception(int error_code, string message, response_type type);
	explicit event_exception(string message);
	virtual const char *what() const throw()
	{
		return message.data();
	};
	const int get_error_code() const
	{
		return error_code;
	}
	const response_type get_type() const
	{
		return type;
	}
	static void error_mounted(int fd, int error_code, const string &error_data, response_type type);

private:
	int error_code;
	string message;
	response_type type;
};

template <typename T>
void throw_exception(int error_code, T message, response_type type)
{

	string error_message = (std::is_same<T, int>::value) ? strerror(errno) : message;
	throw event_exception(error_code, error_message, type);
}

template <typename T>
void throw_exception(T ret_value)
{
	throw event_exception(strerror(errno));
}
#endif
