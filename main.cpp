/**
 * @mainpage OrangeWebserver
 * @author joher 
 * @brief Web服务器
 * @version 0.1
 * @date 2021-03-12
 * 
 * @copyright Copyright (c) 2021
 * 
 */
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
