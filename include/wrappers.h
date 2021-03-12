/**
 * @file wrappers.h
 * @author joher 
 * @brief 包装函数
 * @version 0.1
 * @date 2021-03-12
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <arpa/inet.h>
#include <errno.h>

#include "event_exception.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;
/**
 * @brief 根据errno打印错误信息并结束进程
 * 
 * @param s 错误信息前缀
 */
void error_exit(const char *s);
/**
 * @brief 包装epoll_create
 * @details 仅加入了错误打印功能
 * 
 * @param size 同原函数
 * @return int 同原函数
 */
int Epoll_create(int size);
/**
 * @brief 包装socket
 * @details 仅加入了错误打印功能
 * 
 * @param domain 同原函数
 * @param type 同原函数
 * @param protocol 同原函数
 * @return int 同原函数
 */
int Socket(int domain, int type, int protocol);
/**
 * @brief 包装fcntl
 * @details 仅加入了错误打印功能
 * 
 * @param fd 同原函数
 * @param fd_flag 同原函数
 * @param mode 同原函数
 * @return int 同原函数
 */
int Fcntl(int fd, int fd_flag, int mode);
/**
 * @brief 包装bind
 * @details 仅加入了错误打印功能
 * 
 * @param fd 同原函数
 * @param addr 同原函数
 * @param addr_len 同原函数
 * @return int 同原函数
 */
int Bind(int fd, const struct sockaddr *addr, socklen_t addr_len);
/**
 * @brief 包装listen
 * @details 仅加入了错误打印功能
 * 
 * @param fd 同原函数
 * @param backlog 同原函数
 * @return int 同原函数
 */
int Listen(int fd, int backlog);
/**
 * @brief 包装epoll_wait
 * @details 仅加入了错误打印功能
 * 
 * @param epfd 同原函数
 * @param events 同原函数
 * @param maxevents 同原函数
 * @param timeout 同原函数
 * @return int 调用发生错误时返回0，其他同原函数
 */
int Epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
/**
 * @brief 包装accept
 * @details 增加了错误打印功能并跳过EAGAIN错误
 * 
 * @param sockfd 同原函数
 * @param addr 同原函数
 * @param addrlen 同原函数
 * @return int 同原函数
 */
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
/**
 * @brief 包装epoll_ctl
 * @details 调用失败时打印错误信息并关闭文件描述符
 * @todo 设计调用失败时的返回值
 * 
 * @param epfd 同原函数
 * @param op 同原函数
 * @param fd 同原函数
 * @param event 同原函数
 * @return int 同原函数
 */
int Epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
/**
 * @brief 包装recv
 * @details 跳过EAGAIN错误和EINTR错误，其他错误抛出exception
 * @todo 增加超时处理逻辑并去耦合get_line的逻辑
 * 
 * @param sockfd 同原函数
 * @param buf 同原函数
 * @param len 同原函数
 * @param flags 同原函数
 * @return ssize_t 接收到的字节数
 */
ssize_t Recv(int sockfd, char *buf, size_t len, int flags);
/**
 * @brief 包装open
 * @details 文件打开失败时抛出exception
 * 
 * @param pathname 同原函数
 * @param flags 同原函数
 * @return int 文件描述符
 */
int Open(const char *pathname, int flags);
/**
 * @brief 包装read
 * @details 文件读取失败时抛出exception
 * 
 * @param fd 同原函数
 * @param buf 同原函数
 * @param count 同原函数
 * @return ssize_t 读取到的字节数
 */
ssize_t Read(int fd, void *buf, size_t count);
/**
 * @brief 包装write
 * @details 仅加入了错误打印功能
 * 
 * @param fd 同原函数
 * @param content 同原函数
 * @param fd_name 同原函数
 * @return int 同原函数
 */
int Write(int fd, char *content, string fd_name);
/**
 * @brief 包装close
 * @details 仅加入了错误打印功能
 * 
 * @param fd 同原函数
 * @param fd_name 同原函数
 */
void Close(int fd, string fd_name);
/**
 * @brief 包装stat
 * @details 加入了错误打印功能并抛出exception
 * 
 * @param file_path 文件路径
 * @return mode_t st_mode
 */
mode_t Stat(const char *file_path);
/**
 * @brief 包装pipe
 * @details 加入了错误打印功能并抛出exception，忽略返回值
 * 
 * @param fds 同原函数
 */
void Pipe(int *const fds);
/**
 * @brief 包装wait
 * @details 回收失败时打印错误并抛出exception
 * 
 * @return int 进程退出时的状态码
 */
int Wait();

#endif
