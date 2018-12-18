#include "common.h"
typedef pair<Timer*, Parser* >pairs;
class EventThread
{
private:
	bool bdestroy;
	pthread_t tid;
	int rpipe;
	int wpipe;
	Epoll *pepoll;
	list<int> newconlist;
	unorderd_map<int,pairs> fd2p;
	mutuex conmutuex;
public:
	EventThread();
	~EventThread();
};


	

	
	

