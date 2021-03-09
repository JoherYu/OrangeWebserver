#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <queue>
#include "event_data.h"

class threadpool
{

public:
	threadpool(int min_thr_num, int max_thr_num, int queue_max_size);

	void add_task(event_data *task);

private:
	pthread_mutex_t lock;
	pthread_mutex_t thread_counter;

	pthread_cond_t queue_not_full;
	pthread_cond_t queue_not_empty;

	pthread_t *threads;
	pthread_t adjust_tid;
	queue<event_data *> task_queue;

	int min_thr_num;
	int max_thr_num;
	int live_thr_num;
	int busy_thr_num;
	int wait_exit_thr_num;

	int queue_max_size;

	bool shutdown;

	static void *threadpool_thread(void *pool_obj);
	static void *adjust_thread(void *pool_obj);

	static bool is_thread_alive(pthread_t tid);
	void free_threadpool();
	void destroy_thread();
};

#endif
