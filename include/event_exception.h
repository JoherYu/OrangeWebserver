#ifndef EVENT_EXCEPTION_H
#define EVENT_EXCEPTION_H

#include <stdexcept>
#include <string>

using namespace std;

class event_exception : public exception {

public:
    event_exception(string message);
	virtual const char* what() const throw()
	{
	    return message.data();
	};

private:
	string message;
};

#endif
