#include <unistd.h>
#include "server.h"

int main(int argc, char *argv[])
{
	chdir("./");
	const int a = 1000;
	server<a> instance(5000);
	instance.init();
	instance.start();
}
