compile_flag = main.cpp

ifdef debug
    debug_flag = -g
endif

ifdef test
    compile_flag = func_test.cpp
endif

.PHONY : all everything clean

all : clean everything
clean :
		rm -f demo 
everything :
	g++ -I include/ $(debug_flag) $(compile_flag) event_data.cpp server.cpp http/http.cpp http/http_request.cpp http/http_response.cpp utils.cpp wrappers.cpp event_exception.cpp -o demo
       
       g++ -I include/ $(debug_flag) $(compile_flag) components/login.cpp utils.cpp wrappers.cpp event_exception.cpp http/http_response.cpp http/http.cpp  -o login  

