#include "common.h"
#include "log.h"
const int EPSIZE = 4096；
//ectltype = EPOLL_CTL_DEL,EPOLL_CTL_ADD;
//etype = EPOLLIN
void Epoll::epollCtl(int fd,int events,int ctltype) 
{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = events;
	if(epoll_ctl(epfd,ctltype,fd,&ev) < 0){
		 std::cout<<"error"<<endl;
	}            
}
void Epoll::epollWait() 
{
	fdactive = epoll_wait(epfd,&vfd[0],vfd.size(),-1);
	std::cout<<fdactive<<" active events"<<endl;    
}