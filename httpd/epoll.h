#include "common.h"
#include "log.h"

class Epoll{
private:
	int epfd;
	int rpipe;
	vector<struct epoll_event>evts;
public:
	Epoll(int pfd);
	~Epoll();
	vector<struct epoll_event> epollWait() ;
	
	int addInEvents(int fd);
	
	int delInEvents(int fd);
	
	int addOutEvents(int fd);
	
	int delOutEvents(int fd);
	
}
