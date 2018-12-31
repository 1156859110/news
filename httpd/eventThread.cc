
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
		std::cout<<"v size  "<<v[0].data.fd<<std::endl;
		pevtthread->handleEvents(v);
	}
	pthread_exit(NULL);
}
void EventThread::notifyThread(){
	if(!bnewconn) return;
	std::cout<<"wake up thread"<<std::endl;
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
	std::cout<<"add new connection "<<cbuf[0]<<std::endl;
	std::lock_guard<std::mutex> locker(conmtx);
	bnewconn = false;	
	while(!newconlist.empty()){
		fd = newconlist.front();
		if(pepoll->addInEvents(fd)!=0){
			return ;
		}
		pparser = new Parser(fd);
		ptimer = new Timer(fd);
		/*
		But note that this is not array access; it just looks like it because std::map overloads operator[]. 
		If you do mapping.size() afterwards, you will find that it will be 1.*/
		//fd2pmap[fd] = {pparser,ptimer};这样为啥不行
		fd2pmap[fd] = std::make_pair(ptimer,pparser);
		newconlist.pop_front();
	}	
}
void EventThread::handleEvents(std::vector<struct epoll_event>& evts){
	Timer *ptimer = NULL;
	Parser *pparser = NULL;
	int size = evts.size();
	
	if(size == 0) return;
	for(int i = 0;i < size;i++){
		if(evts[i].data.fd == rpipe){
			addNewCon();
		}else{
			std::cout<<"fd "<<evts[i].data.fd<<std::endl;
			std::pair<Timer*, Parser* >pp = fd2pmap[evts[i].data.fd];
			pparser = pp.second;
			ptimer = pp.first;
			if(evts[i].events & EPOLLIN){
				std::cout<<" read in"<<std::endl;
				pparser->readRequest();
				pparser->getResponse();
				if(!pparser->sendResponse()){
					pepoll->addOutEvents(evts[i].data.fd);
				}
			}
			if(evts[i].events & EPOLLOUT){
				std::cout<<" send outs"<<std::endl;
				if(pparser->sendResponse()){
					pepoll->delOutEvents(evts[i].data.fd);
				}
			}
			ptimer->updateTimer();
		}
		
	}
	//delExpEvents();
	std::cout<<" heap over"<<std::endl;
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
//有个异常啊
void EventThread::delExpEvents(){
	int time = Timer::getTime();
		
	Timer *ptimer = timerheap.getHeap();
	Parser *pparser = NULL;
	std::cout<<"update heap "<<ptimer->getCurexp()<<std::endl;
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
	
	

