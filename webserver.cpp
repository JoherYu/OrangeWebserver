#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <stdio.h>
#include <string.h>
#include "event_data.h"


int main(int argc, char *argv[])
{
    const int MAX_EVENT_NUMBER = 1000;
    const int PORT = 5000;
    int efd = epoll_create(MAX_EVENT_NUMBER+1);
    event_data::set_root(efd);
	int lfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in sin;
	fcntl(lfd, F_SETFL, O_NONBLOCK);
    memset(&sin, 0, sizeof(sin));
    
	sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(PORT);

	bind(lfd, (struct sockaddr *)&sin, sizeof(sin));
	listen(lfd, 20);

	event_data lnode(lfd, acceptconn);
	lnode.mounted(EPOLLIN);
	struct epoll_event events[MAX_EVENT_NUMBER+1];

	while(1){
	    int fd_count = epoll_wait(efd, events, MAX_EVENT_NUMBER+1, 1000);
		for(int i = 0; i < fd_count;i++){
            event_data *node = (event_data*)events[i].data.ptr;
			node->process();
		}

	}
	return 0;
}
