/**
 * @file event_data.h
 * @author joher 
 * @brief epoll反应堆data类
 * @version 0.1
 * @date 2021-03-12
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef EVENT_DATA_H
#define EVENT_DATA_H

#include "http.h"
#include "http_response.h"

#include <sys/epoll.h>

#include <ctime>
#include <memory>
#include <functional>

using namespace std;
/**
 * @brief epoll反应堆data类
 * 
 */
class event_data
{

	friend void acceptconn(event_data &node);
	friend void recvdata(event_data &node);
	friend void senddata(event_data &node);

public:
	/**
	 * @brief 构造函数
	 * @todo 重构
	 * 
	 * @param fd 文件描述符
	 * @param call_back 回调函数
	 */
	event_data(int fd, void (*call_back)(event_data &));
	/**
	 * @brief 构造函数
	 * @todo 重构
	 * 
	 * @param fd 文件描述符
	 * @param call_back 回调函数
	 * @param message HTTP对象
	 */
	event_data(int fd, void (*call_back)(event_data &), shared_ptr<http> message);
	/**
	 * @brief 内联函数：获取文件描述符
	 * 
	 * @return int 文件描述符
	 */
	int get_fd();
	/**
	 * @brief 将当前对象挂载到epoll树节点上
	 * 
	 * @param n_event_name 挂载的epoll事件名称（例：EPOLLOUT）
	 */
	void mounted(int n_event_name);
	/**
	 * @brief 从epoll树上卸载当前对象
	 * 
	 */
	void unmounted();
	/**
	 * @brief 调用回调函数
	 * 
	 */
	void process();
	/**
	 * @brief 内联函数：保存epoll树根节点的文件描述符
	 * 
	 * @param root_fd epoll树根节点的文件描述符
	 */
	static void set_root(int root_fd);

private:
	/**
     * @brief 当前对象的socket文件描述符
     * 
     */
	int fd;
	/**
	 * @brief 当前对象的epoll事件名
	 * 
	 */
	int event_name;
	/**
	 * @brief 当前对象的回调函数
	 * 
	 */
	void (*call_back)(event_data &);
	/**
	 * @brief 当前对象处理的http对象
	 * 
	 */
	shared_ptr<http> message;
	/**
	 * @brief 用于当前对象的挂载和卸载
	 * 
	 */
	int status;
	/**
	 * @brief 暂未使用
	 * 
	 */
	int len;
	/**
	 * @brief 暂未使用
	 * @todo 用于计算超时
	 * 
	 */
	time_t last_active;
	/**
     * @brief epoll树根节点的文件描述符
     * 
     */
	static int epoll_root;
	/**
	 * @brief 互斥锁数组，用于每个socket文件描述符
	 * 
	 */
	static pthread_mutex_t *locks;
	/**
     * @brief 从socket读取一行数据
	 * @todo 精简形参 
	 * @attention 读取错误时会自动挂载错误响应节点，故无需特别处理，直接返回即可
     * 
     * @param node 当前event_data对象
     * @param method 传出参数：HTTP请求方法
     * @param path 传出参数：HTTP请求路径
     * @param protocol 传出参数：HTTP请求协议
     * @return int 读取的字节数
	 * @retval -1 客户端关闭了链接或读取发生错误
     */
	int get_request_line(event_data &node, char *method, char *path, char *protocol);
};

inline void event_data::set_root(int root_fd)
{
	epoll_root = root_fd;
};

inline int event_data::get_fd()
{
	return fd;
}
/**
 * @brief 友元回调函数：accept socket并挂载读节点
 * @todo 核对unmounted作用
 * 
 * @param node 当前event_data对象
 */
void acceptconn(event_data &node);
/**
 * @brief  友元回调函数：recv socket并挂载写节点
 * @todo 重构：fd变量和exception unmounted
 * 
 * @param node 当前event_data对象
 */
void recvdata(event_data &node);
/**
 * @brief 友元回调函数：send socket并挂载读节点
 * @todo 重构：fd变量和exception unmounted
 * 
 * @param node 当前event_data对象
 */
void senddata(event_data &node);

#endif
