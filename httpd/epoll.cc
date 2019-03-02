#include "common.h"
#include "log.h"
#include "epoll.h"

Epoll::Epoll(int pfd):epfd(epoll_create(1)),rpipe(pfd){
	addInEvents(rpipe);
}
Epoll::~Epoll(){
	delInEvents(rpipe);
}
std::vector<struct epoll_event> Epoll::epollWait(int waittime){
	int num = epoll_wait(epfd,&vevent[0],(int)vevent.size(),waittime);
	std::vector<struct epoll_event>v(vevent.begin(),vevent.begin() + num);
	return v; 
}
int Epoll::addInEvents(int fd){
	int rc = 0;
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN;
	vevent.push_back(ev);
	//std::cout<<vevent.size()<<" epoll events "<<std::endl;
	if((rc = epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev)) < 0){
		 LOG_ERROR<<"add epoll in events error";
		 //std::cout<<"add epoll in events error"<<std::endl;
	}     
	return rc;       
}
int Epoll::delInEvents(int fd){
	int rc = 0;
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN;
	vevent.pop_back();
	if((rc = epoll_ctl(epfd,EPOLL_CTL_DEL,fd,&ev)) < 0){
		 LOG_ERROR<<"epoll in del error";
	}     
	return rc;     
}
int Epoll::addOutEvents(int fd){
	int rc = 0;
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN|EPOLLOUT;
	if((rc = epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev)) < 0){
		 LOG_ERROR<<"add out epoll out events  error";
		 //std::cout<<"add out epoll out events  error"<<std::endl;
	}     
	return rc;       
}
int Epoll::delOutEvents(int fd){
	int rc = 0;
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN;
	if((rc = epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev)) < 0){
		 LOG_ERROR<<"del out events error";
		 //std::cout<<"del out events error"<<std::endl;
	}     
	return rc;       
}