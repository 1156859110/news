
#include "common.h"
#include "log.h"
#include "epoll.h"
#include "timer.h"
#include "parser.h"
#include "eventThread.h"
#include "threadPool.h"
#include "dispatch.h"
	//有点2的new方法，，，new EventThread *[threadnum]
ThreadPool::ThreadPool():
threadnum(get_nprocs()*1.5),bdestroy(false),tid(threadnum,0),
pevtthrd(new EventThread *[threadnum]),count(0){
	for(int i = 0; i < threadnum; ++i){
		pevtthrd[i] = new EventThread();
		pthread_create(&tid[i], NULL, pevtthrd[i]->runEvent, NULL);
	}		
}
ThreadPool::ThreadPool(int tnum):
threadnum(tnum),bdestroy(false),tid(threadnum,0),
	pevtthrd(new EventThread *[threadnum]),count(0){
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
	delete[] pevtthrd;
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