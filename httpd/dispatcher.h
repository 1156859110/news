#include "common.h"
#include "threadPoll.h"
#include "log.h"

class Dispatcher{
private:
	int port;
	int listenfd;
	Epoll *pepoll;
	ThreadPoll *pthread;
public:
	int bindListen() ;
	int setNonBlocking(int sockfd);
	void Dispatcher::run();
	Dispatcher();
	~Dispatcher();
}