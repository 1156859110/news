
#include "common.h"
#include "threadPoll.h"

void ThreadPoll::threadPoolCreate()
{
	for(int i = 0; i < threadNum; ++i)
		pthread_create(&threadId[i], NULL, RunTask, NULL);
}
	
void ThreadPoll::AddTask(pFun _fun, int _arg)
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
 
void ThreadPoll::threadPoolDestroy()
{
	if(bdestroy)return;
	destroy = true;
	for(int i = 0; i < threadNum; ++i){
		//线程内部调用exit
		//pthread_exit(NULL);
		pthread_join(threadId[i], NULL);
	}
		
}
int* ThreadPoll::getNextThread()
{
	int tid = count%threadNum;
	++count;
	return threadId[tid];
}
 
void *ThreadPoll::RunTask(void *arg)
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
		
		}
		cout<<"thread "<< pthread_self()<<endl;
		
		if(pool->tasklist.empty()) {
			pthread_mutex_unlock(&(pool->mutex));
			continue;
		}
		curTask = pool->tasklist.front();
		pool->tasklist.pop_front();
		pthread_mutex_unlock(&(pool->mutex));
		//执行任务函数
		(*curTask.fun)(curTask.arg);
	}
}	
	

	
	

