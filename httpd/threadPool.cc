
#include "common.h"
#include "log.h"
#include "epoll.h"
#include "timer.h"
#include "parser.h"
#include "eventThread.h"
#include "threadPool.h"
#include "dispatch.h"

ThreadPool::ThreadPool():
threadnum(get_nprocs()*1.5),bdestroy(false),tid(threadnum,0),
pevtthrd(threadnum,NULL),count(0){
	for(int i = 0; i < threadnum; ++i){
		pevtthrd[i] = new EventThread();
		pthread_create(&tid[i], NULL, pevtthrd[i]->runEvent,pevtthrd[i]);
	}		
}
ThreadPool::ThreadPool(int tnum):
threadnum(tnum),bdestroy(false),tid(threadnum,0),
	pevtthrd(threadnum,NULL),count(0){
	for(int i = 0; i < threadnum; ++i){
		pevtthrd[i] = new EventThread();
		pthread_create(&tid[i], NULL, pevtthrd[i]->runEvent, pevtthrd[i]);
	}	
}
ThreadPool::~ThreadPool(){
	for(int i = 0; i < threadnum; ++i){
		pevtthrd[i]->destroy();
		pthread_join(tid[i], NULL);
		delete pevtthrd[i];
		LOG_DEBUG<<"thread destroy";
		std::cout<<"thread destroy"<< pthread_self()<<std::endl;
	}
}
int ThreadPool::getThreadNum(){
	return threadnum;
}
void ThreadPool::notify(){
	for(int i = 0; i < threadnum; ++i){
		pevtthrd[i]->notifyThread();
	}
};

EventThread* ThreadPool::getEventThread(){
	unsigned int id = count%threadnum;
	++count;
	return pevtthrd[id];
}