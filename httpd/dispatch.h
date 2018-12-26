#ifndef _DISPATCH_H_
#define _DISPATCH_H
class Dispatch{
private:
	int port;
	int listenfd;
	Epoll *pepoll;
	ThreadPool *pthrdpool;
public:
	Dispatch(ThreadPool *pool);
	~Dispatch();
	int bindListen();
	int setNonBlocking(int sockfd);
	void runDispatch();
	
};
#endif