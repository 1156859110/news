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
	std::cout<<evts.size()<<" events size"<<std::endl; 
	int num = epoll_wait(epfd,&evts[0],(int)evts.size(),waittime);
	std::vector<struct epoll_event>v(evts.begin(),evts.begin() + num);
	std::cout<<num<<" active events"<<std::endl; 
	LOG_DEBUG<<num<<" active events\n"; 
	return v; 
}
int Epoll::addInEvents(int fd){
	int rc = 0;
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN;
	evts.push_back(ev);
	std::cout<<evts.size()<<" epoll events "<<std::endl;
	if((rc = epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev)) < 0){
		 LOG_ERROR<<"add in epoll error";
		 std::cout<<"add in epoll error"<<std::endl;
	}     
	return rc;       
}
int Epoll::delInEvents(int fd){
	int rc = 0;
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN;
	evts.pop_back();
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
		 LOG_ERROR<<"add out epoll error";
		 std::cout<<"add out epoll error"<<std::endl;
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
		 std::cout<<"del out events error"<<std::endl;
	}     
	return rc;       
}