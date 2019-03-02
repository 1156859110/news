#ifndef _THREAD_POOL_H_

#define _THREAD_POOL_H_

class ThreadPool
{
public:
	ThreadPool();
	ThreadPool(int tnum);
	~ThreadPool();
	int getThreadNum();
	void notify();
	EventThread* getEventThread();
private:
	bool bdestroy;
	int threadnum;
	std::vector<EventThread *>vethreads;
	std::vector<pthread_t>vtid;
	unsigned int count ;
};

#endif
	

	
	

