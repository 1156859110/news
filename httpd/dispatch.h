#include "common.h"
#include "log.h"

class ThreadPoll;
class Dispatch{
private:
	int port;
	int listenfd;
	Epoll *pepoll;
	ThreadPoll *pthrdpool;
public:
	int bindListen() ;
	int setNonBlocking(int sockfd);
	void run();
	Dispatch(ThreadPoll *pool);
	~Dispatch();
}