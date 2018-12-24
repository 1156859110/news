#include "common.h"
#include "log.h"
#include "timer.h"

const int DEFAULT = 120000;//120s
Timer::Timer(int timeout){
    preexp = getTime() + timeout;
	curexp = preexp;
}
Timer::Timer(){
    preexp = getTime() + DEFAULT;
	curexp = preexp;
}

Timer::~Timer(){
}
int Timer::getTime(){
    struct timeval curtime;
    gettimeofday(&curtime, NULL);
	//只取一天的秒数，防止越界。单位转成ms
    return ((curtime.tv_sec%100000)*1000 + curtime.tv_usec/1000);
}
void Timer::updateTimer(int timeout){
    curexp = getTime() + timeout;
}

void Timer::updateTimer(){
    curexp =  getTime()  + DEFAULT;
}
void Timer::syncTimer(){
	preexp = curexp;
}

int Timer::getCurexp() const{
	return curexp;
}
int Timer::getPreexp() const{
	return preexp;
}
int Timer::getFd() const{
	return fd;
}

Timer* TimerHeap::getHeap(){
	return timerheap.top();
}
void TimerHeap::pushHeap(Timer *ptimer){
	timerheap.push(ptimer);
}
void TimerHeap::popHeap(){
		timerheap.pop();
}