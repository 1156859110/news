
#include "common.h"
#include "threadPoll.h"

EventThread():bdestroy(false),pipefd(1),pepoll(new Epoll(pipefd)){};
~EventThread(){
	delete pepoll;
	close(pipefd);
}
void *EventThread::run(void *arg){
	while(!bdestroy){
		vector<int>v;
		v = pepoll->epollWait();
		for(auto fd:v){
			handle(fd);
		}
	}
	pthread_exit(NULL);
}
void EventThread::destroy(){
	destroy = true;
}


	
	

