#include "common.h"
typedef void (*pFun) (int arg);

class ThreadPoll
{
private:
	bool bdestroy;
	int threadnum;
	EventThread *pevent;
	std::vector<pthread_t>threadId;
	unsigned int count ;
public:
	
	ThreadPool():threadnum(get_nprocs()*1.5),bdestroy(false),threadId(threadnum,0){};
	ThreadPool(int tnum):threadnum(tnum),bdestroy(false),threadId(threadnum,0),pevent(new[threadnum]EventThread()){};
	void PoolDestroy();

};


	

	
	

