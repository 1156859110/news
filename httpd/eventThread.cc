
#include "common.h"
#include "threadPoll.h"



EventThread():bdestroy(false),bnewconn(false){
    int fd[2];
    if(pipe(fd) < 0){
    	LOG_ERROR<<"create pipe error\n"
    }
	rpipe = fd[0];
	wpipe = fd[1];
 	setNonBlocking(rpipe);
	setNonBlocking(wpipe);
	pepoll = new Epoll(rpipe);
};
~EventThread(){
	delete pepoll;
	close(rpipe);
	close(wpipe);
	fd2p.clear();
}

void EventThread::destroy(){
	bdestroy = true;
}

void *EventThread::run(void *arg){
	while(!bdestroy){
		vector<struct epoll_event>v;
		v = pepoll->epollWait(120000);
		handleEvents(v);
	}
	pthread_exit(NULL);
}
void EventThread::notifyThread(){
	if(!bnewconn) return;
	char cbuf='w';
	if(write(wpipe,cbuf,1)<0){
		LOG_ERROR<<"wake up failed\n"
	}		
}
void EventThread::addConList(int fd){
	std::lock_guard<std::mutex> locker(conmtx);
	newconlist.push_back(fd);
	bnewconn = true;	
}
void EventThread::handleNewCon(){
	int fd = 0;
	char cbuf[8];
	Parser *pparser = NULL;
	Timer *ptimer = NULL;
	while(read(rpipe,cbuf,8)>0);
	std::lock_guard<std::mutex> locker(conmtx);
	bnewconn = false;	
	while(!newconlist.empty()){
		fd = newconlist.front();
		if(pepoll->addInEvents(fd)!=0){
			return ;
		}
		pparser = new Parser();
		ptimer = new Timer();
		fd2p[fd] = {pparser,ptimer};
		newconlist.pop_front();
	}	
}
void EventThread::handleEvents(vector<struct epoll_event> &evts){
	Timer *ptimer = NULL;
	Parser *pparser = NULL;
	pairs pp;
	for(auto i:evts){
		if(evts[i].data.fd == rpipe){
			handleNewCon();
		}else{
			pp = fd2p.get(evts[i].data.fd);
			pparser = pp.first;
			ptimer = pp.second;
			if(evts[i].events & EPOLLIN){
				pparser->readRequest();
				if(!pparser->sendResponse()){
					pepoll->addOutEvents(evts[i].data.fd);
				}
			}
			if(evts[i] & EPOLLOUT){
				if(pparser->sendResponse()){
					pepoll->delOutEvents(evts[i].data.fd);
				}
			}
			ptimer->update();
		}
		
	}
	delExpEvents();
}

int EventThread::setNonBlocking(int sockfd) {
    int opts = 0;
	int rc = 0;
    opts = fcntl(sockfd, F_GETFL);
    if(opts < 0) {
    	 LOG_ERROR<<"fcntl get error\n";
		 return opts;
    }
    opts = (opts | O_NONBLOCK);
    if((rc = fcntl(sockfd, F_SETFL, opts)) < 0) {
    	 LOG_ERROR<<"fcntl set error\n";
    }
	return rc;
}

void EventThread::delExpEvents(){
		int time = Timer::getTime();
		
		Timer *ptimer = timerheap.getHeap();
		Parser *pparser = NULL;
		if(ptimer == NULL) return;
		//最小的的都没有超时，不更新堆，直接返回。
		if(ptimer->preexp > time) return;
		timerheap.popHeap();
		ptimer->syncTime();
		if(ptimer->curexp > time){
			timerheap.push(ptimer);
		}
		while((ptimer->curexp < time){
			pparser = fd2p[ptimer->fd].second;
			delete pparser;
			fd2p.erase(ptimer->fd);
			close(ptimer->fd);
			delete ptimer;
			ptimer = timerheap.getHeap();
			timerheap.pop();
		}
	}
	
}
	
	

