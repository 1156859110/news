
#ifndef _EPOLL_H_
#define _EPOLL_H_
class Epoll{
private:
	int epfd;
	int rpipe;
	std::vector<struct epoll_event>evts;
public:
	Epoll(int pfd);
	~Epoll();
	std::vector<struct epoll_event> epollWait(int waittime) ;
	
	int addInEvents(int fd);
	
	int delInEvents(int fd);
	
	int addOutEvents(int fd);
	
	int delOutEvents(int fd);
	
};
#endif
