
#include "common.h"
#include "thread_poll.h"

using namespace std;
struct thread_pool *pool;
void ThreadPool()
{
	pool = new(struct thread_pool);
	pthread_mutex_init(&(pool->mutex), NULL);
	pthread_cond_init(&(pool->cond), NULL);
	pool->bdestroy = false;
	//创建线程数是cpu可用核心数的两倍。
	pool->threadNum = get_nprocs()*2;
	pool->threadId.resize(pool->threadNum);
	for(int i = 0; i < pool->threadNum; ++i)
		pthread_create(&(pool->threadId[i]), NULL, RunTask, NULL);
}
	
void AddTask(pFun _fun, int _arg)
{
	struct task newTask;
	newTask.fun = _fun;
	newTask.arg = _arg;		
	//将任务加到任务链表中
	pthread_mutex_lock(&(pool -> mutex));
	
	pool->tasklist.push_back(newTask);

	pthread_mutex_unlock(&(pool->mutex));
	pthread_cond_signal(&(pool->cond));
}
 
void PoolDestroy()
{
	if(pool->bdestroy)		//防止多次调用该函数
		return;
	pool->bdestroy = true;
	//唤醒所有等待线程，然后销毁线程池
	pthread_cond_broadcast(&(pool->cond));
	
	//回收线程
	for(int i = 0; i < pool->threadNum; ++i)
		pthread_join(pool->threadId[i], NULL);
	//销毁条件变量与互斥量
	pthread_mutex_destroy(&(pool->mutex));
	pthread_cond_destroy(&(pool->cond));

	free(pool);
	pool = NULL;
}
 
void *RunTask(void *arg)
{
	struct task curTask;
	while(true)
	{	
		pthread_mutex_lock(&(pool->mutex));
		if(!pool->bdestroy && pool->tasklist.empty())
		{
			pthread_cond_wait(&(pool->cond), &(pool->mutex));
		}
		if(pool->bdestroy && pool->tasklist.empty())		//销毁线程池时保证任务链表已空
		{
			pthread_mutex_unlock(&(pool -> mutex));
			pthread_exit(NULL);
		}
		cout<<"thread "<< pthread_self()<<endl;
		curTask = pool->tasklist.front();
		pool->tasklist.pop_front();
		pthread_mutex_unlock(&(pool->mutex));
		//执行任务函数
		(*curTask.fun)(curTask.arg);
	}
}	
	

	
	

