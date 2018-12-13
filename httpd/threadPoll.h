#include "common.h"
typedef void (*pFun) (int arg);
//任务函数及参数
struct task
{
	pFun fun;
	int arg;
};
class ThreadPoll
{
private:
	bool bdestroy;
	int threadNum;
	std::vector<pthread_t>threadId;
	int count ;
public:
	void *RunTask(void *arg);
 
	ThreadPool():threadNum(get_nprocs()*2),bdestroy(false),threadId(threadNum,0){};
	
	
	void AddTask(pFun _fun, int _arg);
 
	void PoolDestroy();
 
	void *RunTask(void *arg);
};


	

	
	

