#include "common.h"


class Timer
{
private:
	int preexp;
	int curexp;
	int fd;
public:
	Timer(int timeout);
	Timer();
	~Timer();
	void updateTimer(int timeout);
	void updateTimer();
	void syncTimer();
};

struct cmptimer{
    bool operator() (const Timer*a, const Timer*b ){
	    return a->curexp > b->curexp; 
	}
};
class TimerHeap
{
private:
	std::priority_queue<Timer*,vector<Timer*>, cmptimer>>heap;
public:
	TimerHeap(){};
	~TimerHeap(){};
	Timer* getHeap();
	void pushHeap(Timer *ptimer);
	void popHeap();
};


