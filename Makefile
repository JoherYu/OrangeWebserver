.PHONY : all everything clean

all : clean everything
clean :
		rm -f demo 
everything : 
	g++ -I include/ -g main.cpp event_data.cpp http/http.cpp http/http_request.cpp http/http_response.cpp utils.cpp wrappers.cpp -o demo
