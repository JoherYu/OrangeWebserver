.PHONY : all everything clean

all : clean everything
clean :
		rm -f demo 
everything : 
	g++ -g webserver.cpp event_data.h event_data.cpp http.h http.cpp http_request.h http_request.cpp http_response.h http_response.cpp utils.h utils.cpp -o demo
