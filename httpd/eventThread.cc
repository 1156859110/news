
#include "common.h"
#include "log.h"
#include "timer.h"
#include "epoll.h"
#include "parser.h"
#include "eventThread.h"
const int WAITTIME = 120000;
EventThread::EventThread():bdestroy(false),bnewconn(false){
    int fd[2];
    if(pipe(fd) < 0){
    	LOG_ERROR<<"create pipe error\n";
    }
	rpipe = fd[0];
	wpipe = fd[1];
	//std::cout<<"pipe 0 is "<<fd[0]<< "pipe 1 is "<<fd[1]<<"\n";
	LOG_INFO<<"pipe 0 is "<<fd[0]<< "pipe 1 is "<<fd[1]<<"\n";
 	setNonBlocking(rpipe);
	setNonBlocking(wpipe);
	pepoll = new Epoll(rpipe);    
}
EventThread::~EventThread(){
	delete pepoll;
	close(rpipe);
	close(wpipe);
	mfd2pair.clear();
}

void EventThread::destroy(){
	bdestroy = true;
}
//此处不用声明为static
void* EventThread::runEvent(void *arg){
	EventThread *pevthread = (EventThread *)arg;
	pevthread->tid = syscall(__NR_gettid);
	//std::cout<<"thread running is "<<pevthread->tid<<std::endl;
	LOG_INFO<<"thread running is "<<pevthread->tid<<"\n";
	while(!pevthread->bdestroy){
		std::vector<struct epoll_event>v = pevthread->pepoll->epollWait(WAITTIME);
		pevthread->handleEvents(v);
	}
	pthread_exit(NULL);
}
void EventThread::notifyThread(){
	if(!bnewconn) return;
	//std::cout<<"wake up thread "<<tid<<std::endl;
	LOG_DEBUG<<"wake up thread "<<tid<<"\n";
	char cbuf='w';
	if(write(wpipe,&cbuf,1)<0){
		LOG_ERROR<<tid <<"wake up failed\n";
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
			LOG_ERROR<<"add epoll error task "<<tid<<"fd "<<fd<<"\n";
			return ;
		}
		//std::cout<<tid<<"  thread add new connection fd "<<fd<<std::endl;
		LOG_DEBUG<<tid<<" thread add new connection fd "<<fd<<"\n";
		pparser = new Parser(fd);
		ptimer = new Timer(fd);
		//But note that this is not array access; it just looks like it because std::map overloads operator[]
		mfd2pair[fd] = std::make_pair(ptimer,pparser);
		newconlist.pop_front();
		timerheap.pushHeap(ptimer);
	}	
	////std::cout<<"add new connection end "<<std::endl;
}
void EventThread::handleEvents(std::vector<struct epoll_event>& vevent){
	
	int size = vevent.size();
	bool bdelflag = false;
	if(size == 0) return;
	for(int i = 0;i < size;i++){
		Timer *ptimer = NULL;
		Parser *pparser = NULL;
		////std::cout<<i<<" cur cycle "<<vevent[i].data.fd<<std::endl;
		if(vevent[i].data.fd == rpipe){
			addNewCon();
		}else{
			std::pair<Timer*, Parser* >pp = mfd2pair[vevent[i].data.fd];
			pparser = pp.second;
			ptimer = pp.first;
			if(vevent[i].events & EPOLLIN){
				//std::cout<<"task "<<tid <<" read data fd is "<<vevent[i].data.fd<<std::endl;
				LOG_DEBUG<<"task "<<tid <<" read data fd is "<<vevent[i].data.fd<<"\n";
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
					pepoll->addOutEvents(vevent[i].data.fd);
				}
				if(ptimer != NULL){
					ptimer->updateTimer();
				}
			}
			if(vevent[i].events & EPOLLOUT){
				//std::cout<<"task "<<tid <<" send data fd is "<<vevent[i].data.fd<<std::endl;
				LOG_DEBUG<<"task "<<tid <<" send data fd is"<<vevent[i].data.fd<<"\n";
				if(pparser->sendResponse()){
					pepoll->delOutEvents(vevent[i].data.fd);
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
    	 LOG_ERROR<<tid<<" fcntl get error\n";
		 return opts;
    }
    opts = (opts | O_NONBLOCK);
    if((rc = fcntl(sockfd, F_SETFL, opts)) < 0) {
    	 LOG_ERROR<<tid<<" fcntl set error\n";
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
	//std::cout<<"task "<<tid <<" is update timer heap "<<std::endl;
	LOG_DEBUG<<"task "<<tid <<" is update timer heap \n";
	timerheap.popHeap();
	ptimer->syncTimer();
	timerheap.pushHeap(ptimer);
	ptimer = timerheap.getHeap();
	while(ptimer != NULL && ptimer->getCurexp() < time){
		timerheap.popHeap();
		pparser = mfd2pair[ptimer->getFd()].second;
		delete pparser;
		int fd = ptimer->getFd();
		mfd2pair.erase(fd);
		close(fd);
		pepoll->delInEvents(fd);
		//std::cout<<"task "<<tid <<" close fd "<<fd<<std::endl;
		LOG_DEBUG<<"task "<<tid <<" close fd"<<fd<<"\n";
		delete ptimer;
		ptimer = timerheap.getHeap();
	}
}
	
	

