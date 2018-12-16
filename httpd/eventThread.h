#include "common.h"

class EventThread
{
private:
	bool bdestroy;
	pthread_t tid;
	int pipefd;
	Epoll *pepoll;
public:
	EventThread():bdestroy(false),pipefd(1),pepoll(new Epoll(pipefd)){};
	~EventThread();
};


	

	
	

