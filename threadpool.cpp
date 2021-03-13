#include "threadpool.h"
#include "global.h"
#include <iostream>
#include <cstring>
#include <signal.h>
#include <unistd.h>

threadpool::threadpool(int min_thr_num, int max_thr_num, int queue_max_size) : min_thr_num(min_thr_num), max_thr_num(max_thr_num), live_thr_num(min_thr_num), busy_thr_num(0), wait_exit_thr_num(0), queue_max_size(queue_max_size), shutdown(false)
{
	if (pthread_mutex_init(&lock, NULL) != 0 || pthread_mutex_init(&thread_counter, NULL) != 0 || pthread_cond_init(&queue_not_full, NULL) != 0 || pthread_cond_init(&queue_not_empty, NULL) != 0)
	{
		cout << "fail to init lock or cond" << endl;
		exit(0);
	}

	threads = new pthread_t[max_thr_num];
	memset(threads, 0, sizeof(pthread_t) * max_thr_num);
	for (int i = 0; i < min_thr_num; ++i)
	{
		pthread_create(&threads[i], NULL, threadpool_thread, this);
	}
	pthread_create(&adjust_tid, NULL, adjust_thread, this);
}

void *threadpool::threadpool_thread(void *pool_obj)
{
	threadpool *pool = (threadpool *)pool_obj;
	while (true)
	{
		pthread_mutex_lock(&(pool->lock));
		while ((((pool->task_queue).size()) == 0) && !(pool->shutdown))
		{
			pthread_cond_wait(&(pool->queue_not_empty), &(pool->lock));
			if (pool->wait_exit_thr_num > 0)
			{
				--(pool->wait_exit_thr_num);

				if (pool->live_thr_num > pool->min_thr_num)
				{
					--(pool->live_thr_num);
					pthread_mutex_unlock(&(pool->lock));
					pthread_exit(NULL);
				}
			}
		}

		if (pool->shutdown)
		{
			pthread_mutex_unlock(&(pool->lock));
			pthread_detach(pthread_self());
			pthread_exit(NULL);
		}
		event_data* task = (pool->task_queue).front();
		(pool->task_queue).pop();

		pthread_cond_broadcast(&(pool->queue_not_full));
		pthread_mutex_unlock(&(pool->lock));

		pthread_mutex_lock(&(pool->thread_counter));
		++(pool->busy_thr_num);
		pthread_mutex_unlock(&(pool->thread_counter));

		task->process();

		pthread_mutex_lock(&(pool->thread_counter));
		++(pool->busy_thr_num);
		pthread_mutex_unlock(&(pool->thread_counter));
	}
	pthread_exit(NULL);
}
void *threadpool::adjust_thread(void *pool_obj)
{
	threadpool *pool = (threadpool *)pool_obj;
	int adjust_thread_interval = atoi(conf["adjust_thread_interval"].c_str());
	int min_wait_task_num = atoi(conf["min_wait_task_num"].c_str());
	int thread_vary_num = atoi(conf["thread_vary_num"].c_str());

	while (!(pool->shutdown))
	{

		sleep(adjust_thread_interval);

		pthread_mutex_lock(&(pool->lock));
		int queue_count = (pool->task_queue).size();
		int live_thr_count = pool->live_thr_num;
		pthread_mutex_unlock(&(pool->lock));

		pthread_mutex_lock(&(pool->thread_counter));
		int busy_thr_count = pool->busy_thr_num;
		pthread_mutex_unlock(&(pool->thread_counter));
        /**
         * @brief 线程池扩容，增加thread_vary_num个线程
         * 
         */
		if (queue_count >= min_wait_task_num && live_thr_count < pool->max_thr_num)
		{
			pthread_mutex_lock(&(pool->lock));
			int add = 0;
			for (int i = 0; i < (pool->max_thr_num) && add < thread_vary_num && (pool->live_thr_num) < (pool->max_thr_num); ++i)
			{
				if ((pool->threads)[i] == 0 || !is_thread_alive((pool->threads)[i]))
				{
					pthread_create(&(pool->threads)[i], NULL, threadpool_thread, pool_obj);
					++add;
					++(pool->live_thr_num);
				}
			}
			pthread_mutex_unlock(&(pool->lock));
		}
        /**
         * @brief 销毁thread_vary_num个空闲线程
         * 
         */
		if ((busy_thr_count * 2) < live_thr_count && live_thr_count > (pool->min_thr_num))
		{
			pthread_mutex_lock(&(pool->lock));
			pool->wait_exit_thr_num = thread_vary_num;
			pthread_mutex_unlock(&(pool->lock));

			for (int i = 0; i < thread_vary_num; ++i)
			{
				pthread_cond_signal(&(pool->queue_not_empty));
			}
		}
	}

	return NULL;
}

bool threadpool::is_thread_alive(pthread_t tid)
{
	int result = pthread_kill(tid, 0);
	if (result == ESRCH)
	{
		return false;
	}
	return true;
}
void threadpool::add_task(event_data* task)
{
	pthread_mutex_lock(&lock);

	while (task_queue.size() == queue_max_size)
	{
		pthread_cond_wait(&queue_not_full, &lock);
	}

	if (shutdown)
	{
		pthread_cond_broadcast(&queue_not_empty);
		pthread_mutex_unlock(&lock);
		return;
	}

	task_queue.push(task);
	pthread_cond_signal(&queue_not_empty);

	pthread_mutex_unlock(&lock);
}
void threadpool::free_threadpool()
{
	if (threads)
	{
		delete threads;
		pthread_mutex_lock(&lock);
		pthread_mutex_destroy(&lock);
		pthread_mutex_lock(&thread_counter);
		pthread_mutex_destroy(&thread_counter);
		pthread_cond_destroy(&queue_not_empty);
		pthread_cond_destroy(&queue_not_full);
	}
}
void threadpool::destroy_thread()
{
	shutdown = true;

	pthread_join(adjust_tid, NULL);

	for (int i = 0; i < live_thr_num; ++i)
	{
		pthread_cond_broadcast(&queue_not_empty);
	}
	for (int i = 0; i < live_thr_num; ++i)
	{
		pthread_join(threads[i], NULL);
	}

	free_threadpool();
}
