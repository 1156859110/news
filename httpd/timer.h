#include "common.h"

class Timer
{
private:
    //bool exptime;
	int exptime;
public:
	Timer(int timeout);
	Timer();
	~Timer();
	void update(int timeout);
	void update();
	static std::priority_queue<Timer,vector<Timer>, cmptimer>>timerheap;
};

struct cmptimer{
    bool operator() (const Timer& a, const Timer& b ){
	    return a.exptime > b.exptime; 
	}
};
