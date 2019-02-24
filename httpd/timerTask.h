#ifndef _TIMER_TASK_H_
#define _TIMER_TASK_H_
class Tasks{
public:
	std::function<void()>callback;
	int time;
	int cnt;
	Tasks(std::function<void()>cb,int period):callback(cb),time(period),cnt(period){};
};

class TimerTask{
public:
	static std::vector<Tasks> vtask;
	static void* runTask(void *arg);
	static int  addTimerTask(std::function<void()>cb,int period);
	static void start();
};
#endif




