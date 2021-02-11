#include "http_request.h"

http_request::http_request(string method, string url, string protocol) : method(method), url(url),
																		 http(protocol, "", "") {}
