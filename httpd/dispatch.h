#ifndef _DISPATCH_H_
#define _DISPATCH_H
class Dispatch{

public:
	Dispatch(ThreadPool *pool);
	~Dispatch();
	int bindListen();
	int setNonBlocking(int sockfd);
	void runDispatch();
private:
	int port;
	int listenfd;
	Epoll *pepoll;
	ThreadPool *pthrdpool;
	
};
#endif