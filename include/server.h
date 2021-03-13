/**
 * @file server.h
 * @author joher 
 * @brief TCP/IP 服务器
 * @version 0.1
 * @date 2021-03-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef SERVER_H
#define SERVER_H

#include <sys/epoll.h>
#include <iostream>

#include <threadpool.h>

using namespace std;
/**
 * @brief 服务器类
 * 
 */
class server
{
public:
    /**
     * @brief 构造函数：初始化除epoll_fd外的全部变量
     * 
     * @param port 监听端口
     * @param max_event_number 最大监听事件数
     */
	server(int port, int max_event_number);
	/**
	 * @brief 初始化socket连接并挂载初始节点
	 * 
	 */
	void init();
	/**
	 * @brief 循环监听socket事件
	 * 
	 * @param pool 线程池对象
	 */
	void start(threadpool &pool);

private:
    /**
     * @brief 监听端口
     * 
     */
	const int PORT;
	/**
	 * @brief 最大监听事件数
	 * 
	 */
	const int MAX_EVENT_NUMBER;
	/**
	 * @brief epoll根节点文件描述符
	 * 
	 */
	int epoll_fd;
	/**
	 * @brief epoll_event数组
	 * 
	 */
	struct epoll_event *events;
	
};

#endif
