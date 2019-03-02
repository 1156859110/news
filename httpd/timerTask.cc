#include "common.h"
#include "timerTask.h"
#include "log.h"

std::vector<Tasks> TimerTask::vtask;
const int periodtimer = 10;//10s
int  TimerTask::addTimerTask(std::function<void()>cb,int period){
	Tasks t(cb,period);
	vtask.push_back(t);
	return 0;
}
//在线程里面执行的函数不能是持有对象的，会导致参数名不一致
void* TimerTask::runTask(void *arg)
{
	//std::vector<Tasks> *pvtask = (std::vector<Tasks> *)arg;
	int size = vtask.size();
	if(size ==0) return NULL;
	while (true){
		sleep(periodtimer);
		for(int i = 0;i< size;++i){
			if(vtask[i].cnt--<=0){
				vtask[i].cnt = vtask[i].time;
				vtask[i].callback();
			}
		}
	}
	return NULL;
}
void TimerTask::start(){
	pthread_t tid;
	pthread_create(&tid, NULL,TimerTask::runTask,NULL);
	//std::cout<<"timer task id is "<< tid<<std::endl;
	LOG_INFO<<"timer task id is "<< tid<<"\n";
}
		   
				   
    



















