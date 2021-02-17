#include <unistd.h>
#include "server.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    map<string, string> conf = get_conf("init.conf"); 
	chdir("./");
	//const int MAX_EVENT_NUMBER = 1000;
	server instance(atoi(conf["port"].data()), atoi(conf["max_event_number"].data()));
	instance.init();
	instance.start();
}
