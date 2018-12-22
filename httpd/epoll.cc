#include "common.h"
#include "log.h"

Epoll(int pfd):epfd(epoll_create(1),rpipe(pfd){
	addInEvents(rpipe);
}
~Epoll(){
	delInEvents(rpipe);
}
vector<struct epoll_event> Epoll::epollWait(int waittime) 
{
	int num = epoll_wait(epfd,&evts[0],evts.size(),waittime);
	vector<struct epoll_event>v(evts,evts+num);
	std::cout<<fdactive<<" active events"<<endl; 
	LOG_DEBUG<<fdactive<<" active events"; 
	return v; 
}
int Epoll::addInEvents(int fd){
	int rc = 0;
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN;
	if((rc = epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev)) < 0){
		 LOG_ERROR<<"add in epoll error"
	}     
	return rc;       
}
int Epoll::delInEvents(int fd){
	int rc = 0;
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN;
	if((rc = epoll_ctl(epfd,EPOLL_CTL_DEL,fd,&ev)) < 0){
		 LOG_ERROR<<"epoll in del error"
	}     
	return rc;     
}
int Epoll::addOutEvents(int fd){
	int rc = 0;
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN |  EPOLLOUT;
	if((rc = epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev)) < 0){
		 LOG_ERROR<<"add out epoll error"
	}     
	return rc;       
}
int Epoll::delOutEvents(int fd){
	int rc = 0;
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN;
	if((rc = epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev)) < 0){
		 LOG_ERROR<<"del out events error"
	}     
	return rc;       
}