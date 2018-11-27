#include "common.h"
typedef void (*pFun) (int arg);
//任务函数及参数
struct task
{
	pFun fun;
	int arg;
};
struct thread_pool
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	std::list<struct task>tasklist;
	//是否销毁线程池
	bool bdestroy;
	int threadNum;
	std::vector<pthread_t>threadId;
};
void *RunTask(void *arg);
 
void ThreadPool();
	
void AddTask(pFun _fun, int _arg);
 
void PoolDestroy();
 
void *RunTask(void *arg);

	

	
	

