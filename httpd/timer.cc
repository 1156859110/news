#include "common.h"

const int DEFAULT = 300;
Timer::Timer(int timeout){
    struct timeval curtime;
    gettimeofday(&curtime, NULL);
    exptime = curtime.tv_sec + curtime.tv_usec/1000 + timeout;
}
Timer::Timer(){
    struct timeval curtime;
    gettimeofday(&curtime, NULL);
    exptime = curtime.tv_sec + curtime.tv_usec/1000 + DEFAULT;
}

Timer::~Timer(){
}

void Timer::update(int timeout){
    struct timeval curtime;
    gettimeofday(&curtime, NULL);
    exptime = curtime.tv_sec + curtime.tv_usec/1000 + timeout;
}

void Timer::update(){
    struct timeval curtime;
    gettimeofday(&curtime, NULL);
    exptime = curtime.tv_sec + curtime.tv_usec/1000 + DEFAULT;
}
static void Timer::delExpTimer(){
		timeheap.pop();
}
static void Timer::addTimer(Timer &t){
		timerheap.push(t);
}
