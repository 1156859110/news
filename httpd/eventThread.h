#include "common.h"
typedef pair<Timer*, Parser* >pairs;
class EventThread
{
private:
	bool bdestroy;
	bool bnewconn;
	pthread_t tid;
	int rpipe;
	int wpipe;
	Epoll poll;
	TimerHeap timerheap;
	list<int> newconlist;
	unorderd_map<int,pairs> fd2p;
	std::mutex conmtx;
	
public:
	EventThread();
	~EventThread();
	int setNonBlocking(int sockfd);
};


	

	
	

