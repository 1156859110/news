#ifndef _EVENT_THREAD_H_
#define _EVENT_THREAD_H_
class EventThread
{
private:
	bool bdestroy;
	bool bnewconn;
	pthread_t tid;
	int rpipe;
	int wpipe;
	Epoll *pepoll;
	TimerHeap timerheap;
	std::list<int> newconlist;
	std::unordered_map<int,std::pair<Timer*, Parser* >> fd2pmap;
	std::mutex conmtx;
	
public:
	EventThread();
	~EventThread();
	
	void destroy();
	void notifyThread();
	void addConList(int fd);
	void addNewCon();
	void handleEvents(std::vector<struct epoll_event> &evts);

	int setNonBlocking(int sockfd);

	void delExpEvents();
	static void* runEvent(void *arg);
};

#endif
	

	
	

