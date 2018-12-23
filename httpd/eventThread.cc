
#include "common.h"
#include "log.h"
#include "timer.h"
#include "epoll.h"
#include "parser.h"
#include "eventThread.h"

EventThread::EventThread():bdestroy(false),bnewconn(false){
    int fd[2];
    if(pipe(fd) < 0){
    	LOG_ERROR<<"create pipe error\n";
    }
	rpipe = fd[0];
	wpipe = fd[1];
 	setNonBlocking(rpipe);
	setNonBlocking(wpipe);
	pepoll = new Epoll(rpipe);       
}
EventThread::~EventThread(){
	delete pepoll;
	close(rpipe);
	close(wpipe);
	fd2pmap.clear();
}

void EventThread::destroy(){
	bdestroy = true;
}
//此处不用声明为static
void* EventThread::runEvent(void *arg){
	EventThread *pevtthread = (EventThread *)arg;
	while(!pevtthread->bdestroy){
		std::vector<struct epoll_event>v;
		v = pevtthread->pepoll->epollWait(120000);
		pevtthread->handleEvents(v);
	}
	pthread_exit(NULL);
}
void EventThread::notifyThread(){
	if(!bnewconn) return;
	char cbuf='w';
	if(write(wpipe,&cbuf,1)<0){
		LOG_ERROR<<"wake up failed\n";
	}		
}
void EventThread::addConList(int fd){
	std::lock_guard<std::mutex> locker(conmtx);
	newconlist.push_back(fd);
	bnewconn = true;	
}
void EventThread::addNewCon(){
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
		//fd2pmap[fd] = {pparser,ptimer};这样为啥不行
		//fd2pmap.insert({fd,{pparser,ptimer}});
		 fd2pmap.emplace(fd,{ptimer,pparser}); 
		newconlist.pop_front();
	}	
}
void EventThread::handleEvents(std::vector<struct epoll_event> &evts){
	Timer *ptimer = NULL;
	Parser *pparser = NULL;
	for(auto event:evts){
		if(event.data.fd == rpipe){
			addNewCon();
		}else{
			std::pair<Timer*, Parser* >pp = fd2pmap[event.data.fd)];
			pparser = pp.second;
			ptimer = pp.first;
			if(event.events & EPOLLIN){
				pparser->readRequest();
				if(!pparser->sendResponse()){
					pepoll->addOutEvents(event.data.fd);
				}
			}
			if(evts[i] & EPOLLOUT){
				if(pparser->sendResponse()){
					pepoll->delOutEvents(event.data.fd);
				}
			}
			ptimer->updateTimer();
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
		if(ptimer->getPreexp() > time) return;
		timerheap.popHeap();
		ptimer->syncTimer();
		if(ptimer->getCurexp() > time){
			timerheap.pushHeap(ptimer);
		}
		while(ptimer->getCurexp() < time){
			pparser = fd2pmap[ptimer->getFd()].second;
			delete pparser;
			fd2pmap.erase(ptimer->getFd());
			close(ptimer->getFd());
			delete ptimer;
			ptimer = timerheap.getHeap();
			timerheap.popHeap();
		}
	}
	
}
	
	

