#include "server.h"
#include "utils.h"
#include "global.h"
#include "threadpool.h"

#include <unistd.h>

int main(int argc, char *argv[])
{
	get_conf("init.conf", conf);
	chdir(conf["static_file_dir"].data());
	server instance(atoi(conf["port"].data()), atoi(conf["max_event_number"].data()));
	threadpool t_pool(3, 100, 100); //todo
	instance.init();
	instance.start(t_pool);
}
