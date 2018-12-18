
#include "common.h"
#include "threadPoll.h"



EventThread():bdestroy(false){
    int fd[2];
    if(pipe(fd) < 0){
    	LOG_ERROR<<"create pipe error\n"
    }
	rpipe = fd[0];
	wpipe = fd[1];
	pepoll = new Epoll(rpipe);
};
~EventThread(){
	delete pepoll;
	close(rpipe);
	close(wpipe);
	fd2p.clear();
}

void EventThread::destroy(){
	destroy = true;
}

void *EventThread::run(void *arg){
	while(!bdestroy){
		vector<struct epoll_event>v;
		v = pepoll->epollWait();
		handleEvents(v);
	}
	pthread_exit(NULL);
}

void EventThread::addNewCon(){
	int fd = 0;
	char cbuf[8];
	while(read(rpipe,cbuf,8)>0);
	//mutuex
	conmtx.lock();
	while(!newconlist.empty()){
		fd = newconlist.front();
		if(pepoll->addInEvents(fd)!=0){
			conmtx.unlock();
			return ;
		};
		newconlist.popfront();
	}
	conmtx.unlock();
			
}
void EventThread::handleEvents(vector<struct epoll_event> &evts){
	Timer *ptimer = NULL;
	Parser *pparser = NULL;
	pairs pp;
	for(auto i:evts){
		pp = fd2p.get(evts[i].data.fd);
		pparser = pp.first();
		ptimer = pp.second();
		if(evts[i].data.fd == rpipe){
			addNewCon();
		}else{
			if(evts[i].events & EPOLLIN){
				if(pparser->parser(fd) == FINISHED){
					if(pparser->response(fd)!=0){
						pepoll->addOutEvents(fd);
					}
					//出错加入epoll
				}
			}
			if(evts[i] & EPOLLOUT){
				if(pparser->response(fd)!=0){
					pepoll->addOutEvents(fd);
				}
				pepoll->delOutEvents(fd);
			}
		}
		
	}
}




	
	
void EventThread::delExp(){
		int time = getTime();
		Timer *ptimer = Timer::timerheap.top();
		//最小的的都没有超时，不更新堆，直接返回。
		if(ptimer->exptime > time && ) return;
		Timer::timerheap.pop();
		if(ptimer->expupdate > time){
			Timer::timerheap.push(ptimer);
		}
		while(Timer::timerheap.top() < time){
			Timer::timerheap.pop();
			close(fd);
			delete parser;
			delete timer;
		}
	}
	
}
	
	

