#include "common.h"
#include "log.h"

class {
private:
	int epfd;
	int pipefd;
	int fdactive;
	vector<int>vfd;
public:
	Epoll():epfd(epoll_create(EPSIZE)){
		epollCtl(pipefd,EPOLLIN,EPOLL_CTL_ADD);
	}
	~Epoll(){
		epollCtl(pipefd,EPOLLIN,EPOLL_CTL_DEL);
		close(pipefd);
	}
	void epollCtl(int fd,int events,int ctltype) {};
	void epollWait() {};
}
