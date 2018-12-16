#include "common.h"
#include "log.h"

class Epoll{
private:
	int epfd;
	int pipefd;
	int fdsize;
	vector<int>vfd;
public:
	Epoll(int fd):epfd(epoll_create(1),pipefd(fd){
		epollCtl(pipefd,EPOLLIN,EPOLL_CTL_ADD);
	}
	Epoll():epfd(epoll_create(1){};
	~Epoll(){
		epollCtl(pipefd,EPOLLIN,EPOLL_CTL_DEL);
		close(pipefd);
	}
	void epollCtl(int fd,int events,int ctltype) {};
	void epollWait();
	int getFdSize();
}
