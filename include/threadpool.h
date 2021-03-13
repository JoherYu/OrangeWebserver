/**
 * @file threadpool.h
 * @author joher 
 * @brief 线程池
 * @version 0.1
 * @date 2021-03-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <queue>
#include "event_data.h"
/**
 * @brief 线程池类
 * 
 */
class threadpool
{
public:
    /**
     * @brief 初始化线程池并启动一个管理线程和min_thr_num个普通线程
     * @todo 错误处理（退出进程还是free）
	 * 
     * @param min_thr_num 最小线程数
     * @param max_thr_num 最大线程数
     * @param queue_max_size 最大任务数
     */
	threadpool(int min_thr_num, int max_thr_num, int queue_max_size);
    /**
     * @brief 添加任务到任务队列
     * 
     * @param task 
     */
	void add_task(event_data *task);

private:
    /**
     * @brief 线程池所有成员变量的公共锁
     * 
     */
	pthread_mutex_t lock;
	/**
	 * @brief busy_thr_num变量的锁
	 * 
	 */
	pthread_mutex_t thread_counter;
    /**
     * @brief 条件变量：用于向任务队列中添加任务
     * 
     */
	pthread_cond_t queue_not_full;
	/**
	 * @brief 条件变量：用于向任务队列中添加任务，销毁多余线程
	 * 
	 */
	pthread_cond_t queue_not_empty;
    /**
     * @brief 线程池ID数组
     * 
     */
	pthread_t *threads;
	/**
	 * @brief 管理线程
	 * 
	 */
	pthread_t adjust_tid;
	/**
	 * @brief 任务队列
	 * 
	 */
	queue<event_data *> task_queue;
    /**
     * @brief 最小线程数
     * 
     */
	int min_thr_num;
	/**
	 * @brief 最大线程数
	 * 
	 */
	int max_thr_num;
	/**
	 * @brief 当前线程数
	 * 
	 */
	int live_thr_num;
	/**
	 * @brief 正在执行任务的线程数
	 * 
	 */
	int busy_thr_num;
	/**
	 * @brief 需要销毁的线程个数
	 * 
	 */
	int wait_exit_thr_num;
    /**
     * @brief 最大任务数
     * 
     */
	int queue_max_size;
    /**
     * @brief 线程池销毁标志/状态
     * 
     */
	bool shutdown;
    /**
     * @brief 普通工作线程回调函数
	 * @attention 回调函数类型必须为void *FUC(void *)，无法使用普通成员函数
	 * 
     * @param pool_obj 线程池指针
     * @return void* 无意义
     */
	static void *threadpool_thread(void *pool_obj);
	/**
	 * @brief 管理线程回调函数
	 * @attention 回调函数类型必须为void *FUC(void *)，无法使用普通成员函数
	 * 
	 * @param pool_obj 线程池指针
	 * @return void* 无意义
	 */
	static void *adjust_thread(void *pool_obj);
    /**
     * @brief 判断线程时候存活
     * 
     * @param tid 线程ID
     * @return true 线程存活
     * @return false 线程死亡
     */
	static bool is_thread_alive(pthread_t tid);
	/**
	 * @brief 释放线程池（变量）
	 * 
	 */
	void free_threadpool();
	/**
	 * @brief 销毁线程池，包括回收线程，释放线程池空间
	 * 
	 */
	void destroy_thread();
};

#endif
