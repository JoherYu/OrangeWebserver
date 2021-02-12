#include <unistd.h>
#include "server.h"

int main(int argc, char *argv[])
{
	chdir("./");
	const int MAX_EVENT_NUMBER = 1000;
	server<MAX_EVENT_NUMBER> instance(5000);
	instance.init();
	instance.start();
}
