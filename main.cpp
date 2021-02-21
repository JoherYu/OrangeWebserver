#include "server.h"
#include "utils.h"

#include <unistd.h>

int main(int argc, char *argv[])
{
	map<string, string> conf;
	get_conf("init.conf", conf);
	chdir(conf["static_file_dir"].data());
	server instance(atoi(conf["port"].data()), atoi(conf["max_event_number"].data()));
	instance.init();
	instance.start();
}
