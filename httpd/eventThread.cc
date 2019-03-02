
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
	std::cout<<fd[0]<<"pipe is "<<fd[1]<<"\n";
	LOG_INFO<<fd[0]<<"pipe is "<<fd[1]<<"\n";
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
		v = pevtthread->pepoll->epollWait(5000);
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
	std::cout<<"thread running is "<<syscall(__NR_gettid)<<std::endl;
	std::lock_guard<std::mutex> locker(conmtx);
	bnewconn = false;	
	while(!newconlist.empty()){
		fd = newconlist.front();
		if(pepoll->addInEvents(fd)!=0){
			return ;
		}
		std::cout<<"timer heap add new connection fd "<<fd<<std::endl;
		pparser = new Parser(fd);
		ptimer = new Timer(fd);
		/*
		But note that this is not array access; it just looks like it because std::map overloads operator[]. 
		If you do mapping.size() afterwards, you will find that it will be 1.*/
		//fd2pmap[fd] = {pparser,ptimer};这样为啥不行
		fd2pmap[fd] = std::make_pair(ptimer,pparser);
		newconlist.pop_front();
		timerheap.pushHeap(ptimer);
	}	
	std::cout<<"add new connection end "<<std::endl;
}
void EventThread::handleEvents(std::vector<struct epoll_event>& evts){
	
	int size = evts.size();
	std::cout<<"thread "<<syscall(__NR_gettid)<<" is handle events "<<std::endl;
	bool bdelflag = false;
	if(size == 0) return;
	for(int i = 0;i < size;i++){
		Timer *ptimer = NULL;
		Parser *pparser = NULL;
		//std::cout<<i<<" cur cycle "<<evts[i].data.fd<<std::endl;
		if(evts[i].data.fd == rpipe){
			addNewCon();
		}else{
			std::cout<<"active fd is "<<evts[i].data.fd<<std::endl;
			std::pair<Timer*, Parser* >pp = fd2pmap[evts[i].data.fd];
			pparser = pp.second;
			ptimer = pp.first;
			if(evts[i].events & EPOLLIN){
				std::cout<<" read in"<<std::endl;
				if(!pparser->readRequest()){
					//说明连接已经关闭，这里置为过期，如果有一个过期事件，后续不在调整堆。
					ptimer->setTimeout();
					if(!bdelflag){
						bdelflag = true;
						Timer *ptemp = timerheap.getHeap();
						if(ptemp != NULL){
							timerheap.popHeap();
							timerheap.pushHeap(ptemp);
						}
					}
					continue;
				}
				if(!pparser->sendResponse()){
					pepoll->addOutEvents(evts[i].data.fd);
				}
				if(ptimer != NULL){
					ptimer->updateTimer();
				}
			}
			if(evts[i].events & EPOLLOUT){
				std::cout<<" send outs"<<std::endl;
				if(pparser->sendResponse()){
					pepoll->delOutEvents(evts[i].data.fd);
				}
			}
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
	std::cout<<"update heap "<<ptimer->getCurexp()<<std::endl;
	//最小的的都没有超时，不更新堆，直接返回。
	if(ptimer->getPreexp() > time) return;
	
	timerheap.popHeap();
	ptimer->syncTimer();
	timerheap.pushHeap(ptimer);
	ptimer = timerheap.getHeap();
	while(ptimer != NULL && ptimer->getCurexp() < time){
		timerheap.popHeap();
		pparser = fd2pmap[ptimer->getFd()].second;
		delete pparser;
		int fd = ptimer->getFd();
		fd2pmap.erase(fd);
		close(fd);
		pepoll->delInEvents(fd);
		std::cout<<"close fd "<<fd<<std::endl;
		delete ptimer;
		ptimer = timerheap.getHeap();
	}
}
	
	

