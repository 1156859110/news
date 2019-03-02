
#include "common.h"
#include "log.h"
#include "epoll.h"
#include "timer.h"
#include "parser.h"
#include "eventThread.h"
#include "threadPool.h"
#include "dispatch.h"

ThreadPool::ThreadPool():
	threadnum(get_nprocs()*1.5),
	bdestroy(false),
	vtid(threadnum,0),
	vethreads(threadnum,NULL),
	count(0){
	for(int i = 0; i < threadnum; ++i){
		vethreads[i] = new EventThread();
		pthread_create(&vtid[i], NULL, vethreads[i]->runEvent,vethreads[i]);
		LOG_INFO<<"task "<<i<<" id is "<<vtid[i]<<"\n";
	}		
}
ThreadPool::ThreadPool(int tnum):
	threadnum(tnum),
	bdestroy(false),
	vtid(threadnum,0),
	vethreads(threadnum,NULL),
	count(0){
	for(int i = 0; i < threadnum; ++i){
		vethreads[i] = new EventThread();
		pthread_create(&vtid[i], NULL, vethreads[i]->runEvent, vethreads[i]);
		LOG_INFO<<"task "<<i<<" id is "<<vtid[i]<<"\n";
	}	
}
ThreadPool::~ThreadPool(){
	for(int i = 0; i < threadnum; ++i){
		vethreads[i]->destroy();
		pthread_join(vtid[i], NULL);
		delete vethreads[i];
		LOG_INFO<<vtid[i]<<" thread destroy \n";
		//std::cout<<vtid[i]<<" thread destroy \n";
	}
}
int ThreadPool::getThreadNum(){
	return threadnum;
}
void ThreadPool::notify(){
	for(int i = 0; i < threadnum; ++i){
		vethreads[i]->notifyThread();
	}
}

EventThread* ThreadPool::getEventThread(){
	unsigned int id = count % threadnum;
	++count;
	return vethreads[id];
}