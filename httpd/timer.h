#ifndef _TIMER_H_
#define _TIMER_H_

class Timer
{
public:
	Timer(int ifd,int timeout);
	Timer(int ifd);
	~Timer();
	void updateTimer(int timeout);
	void updateTimer();
	void syncTimer();
	void setTimeout();
	int getCurexp() const;
	int getPreexp() const;
	int getFd() const;
	static int getTime();
private:
	int preexp;
	int curexp;
	int fd;
};

struct cmptimer{
    bool operator() (const Timer*a, const Timer*b ){
	    return a->getCurexp() > b->getCurexp(); 
	}
};
class TimerHeap
{
public:
	TimerHeap(){};
	~TimerHeap(){};
	Timer* getHeap();
	void pushHeap(Timer *ptimer);
	void popHeap();
private:
	std::priority_queue<Timer*,std::vector<Timer*>, cmptimer>timerheap;
};
#endif

