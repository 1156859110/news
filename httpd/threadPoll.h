#include "common.h"

class EventThread;
class ThreadPoll
{
private:
	bool bdestroy;
	int threadnum;
	EventThread *pevtthrd;
	std::vector<pthread_t>tid;
	unsigned int count ;
public:
	ThreadPool();
	ThreadPool(int tnum);
	~ThreadPool();
	int getThreadNum();
	void notify();
	EventThread* getEventThread();
};


	

	
	

