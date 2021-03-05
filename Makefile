compile_flag = g++
test_flag = main.cpp
include_flag = -I include/
programme_name = demo

objs = event_data.o server.o http.o http_request.o http_response.o utils.o wrappers.o event_exception.o

ifdef debug
    debug_flag = -g
endif

ifdef test
    test_flag = func_test.cpp
endif

.PHONY : all everything clean

all : everything
clean : 
	rm -f $(programme_name) 
allclean : 
	rm -f $(programme_name) $(objs)
everything : $(programme_name)
components : components/login
	#$(compile_flag) $(include_flag) $(debug_flag) $(test_flag) event_data.cpp server.cpp http/http.cpp http/http_request.cpp http/http_response.cpp utils.cpp wrappers.cpp event_exception.cpp -o demo
	
event_data.o: event_data.cpp
	$(compile_flag) $(include_flag) $(debug_flag) -c $<

server.o: server.cpp
	$(compile_flag) $(include_flag) $(debug_flag) -c $<

http.o: http/http.cpp
	$(compile_flag) $(include_flag) $(debug_flag) -c $<

http_request.o: http/http_request.cpp
	$(compile_flag) $(include_flag) $(debug_flag) -c $<

http_response.o: http/http_response.cpp
	$(compile_flag) $(include_flag) $(debug_flag) -c $<

utils.o: utils.cpp
	$(compile_flag) $(include_flag) $(debug_flag) -c $<

wrappers.o: wrappers.cpp
	$(compile_flag) $(include_flag) $(debug_flag) -c $<

event_exception.o: event_exception.cpp
	$(compile_flag) $(include_flag) $(debug_flag) -c $<
	
$(programme_name): $(test_flag) $(objs)
	$(compile_flag) $(include_flag) $(debug_flag) $(test_flag) $(objs) -o $(programme_name)

components/login: components/login.cpp $(objs)
	$(compile_flag) $(include_flag) $(debug_flag) components/login.cpp $(objs)  `mysql_config --cflags --libs` -o components/login