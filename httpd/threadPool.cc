
#include "common.h"
#include "eventThread.h"
#include "threadPool.h"

	
ThreadPool::ThreadPool():
threadnum(get_nprocs()*1.5),bdestroy(false),tid(threadnum,0),
pevtthrd(new[threadnum]EventThread()),count(0){
	for(int i = 0; i < threadnum; ++i){
		pthread_create(&tid[i], NULL, pevtthrd[i]->runEvent, NULL);
	}		
}
ThreadPool::ThreadPool(int tnum):
threadnum(tnum),bdestroy(false),tid(threadnum,0),
pevtthrd(new[threadnum]EventThread()),count(0){
	for(int i = 0; i < threadnum; ++i){
		pthread_create(&tid[i], NULL, pevtthrd[i]->runEvent, pevtthrd[i]);
	}	
}
void ThreadPool::~ThreadPool(){
	for(int i = 0; i < threadnum; ++i){
		pevtthrd[i]->destroy();
		pthread_join(tid[i], NULL);
		LOG_DEBUG<<"thread destroy";
		cout<<"thread destroy"<< pthread_self()<<endl;
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