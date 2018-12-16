
#include "common.h"
#include "threadPoll.h"

void ThreadPoll::create()
{
	for(int i = 0; i < threadnum; ++i){
		pthread_create(&threadId[i], NULL, pevent[i]->run, NULL);
	}	
}
void ThreadPoll::destroy()
{
	if(bdestroy)return;
	destroy = true;
	for(int i = 0; i < threadnum; ++i){
		pevent[tid]->destroy();
		//需要等待线程退出吗？
		pthread_join(threadId[i], NULL);
		cout<<"thread "<< pthread_self()<<endl;
	}
}
EventThread* ThreadPoll::getEventThread(){
	unsigned int tid = count%threadnum;
	++count;
	return pevent[tid];
}