#include "event_data.h"
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <memory.h>
#include <stdio.h>
#include <iostream>

using std::cout;

int event_data::epoll_root = 0;

event_data::event_data(int fd, void (*call_back)(event_data&)):
    fd(fd), call_back(call_back), event_name(0), status(0), len(0)
{
    memset(buf, 0, sizeof(buf));
	last_active = time(NULL);
}

void event_data::process()
{
    (*call_back)(*this);
}

void event_data::mounted(int n_event_name)
{
    struct epoll_event epv = {0, {0}};
    int op;
	epv.data.ptr = this;
	epv.events = event_name = n_event_name;

	if(status == 0){
	    op = EPOLL_CTL_ADD;
		status = 1;
	}

	epoll_ctl(epoll_root, op, fd, &epv);
}

void event_data::unmounted()
{
    // struct epoll_event epv = {0, {0}};
	if(status != 1) return;
	// epv.data.ptr = NULL;
	status = 0;
	epoll_ctl(epoll_root, EPOLL_CTL_DEL, fd, NULL);
}

void event_data::set_root(int root_fd)
{
   epoll_root = root_fd; 
};

void acceptconn(event_data& node)
{
    struct sockaddr_in cin;
	socklen_t len = sizeof(cin);
	int cfd = accept(node.fd, (struct sockaddr *)&cin, &len);
	fcntl(cfd, F_SETFL, O_NONBLOCK);
	event_data cnode(cfd, recvdata);
    cnode.mounted(EPOLLIN);
	return;
};

void recvdata(event_data& node)
{
    int len = recv(node.fd, node.buf, sizeof(node.buf), 0);
	cout << node.buf;
	node.unmounted();
	
	if(len > 0){
		event_data w_node = event_data(node.fd, senddata);
		strcpy(w_node.buf, "aaa");
		w_node.mounted(EPOLLOUT);
	}else if(len == 0){
		close(node.fd);
	}else{
	    close(node.fd);
	}
};

void senddata(event_data& node)
{
    int len = send(node.fd, node.buf, sizeof(node.buf), 0);
	node.unmounted();
	
	if(len > 0){
		event_data w_node = event_data(node.fd, recvdata);
		w_node.mounted(EPOLLIN);
	}else{
	    close(node.fd);
	}
	return;
}
