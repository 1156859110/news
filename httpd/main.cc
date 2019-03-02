#include "common.h"
#include "log.h"
#include "cache.h"
#include "timer.h"
#include "epoll.h"
#include "parser.h"
#include "eventThread.h"
#include "threadPool.h"
#include "dispatch.h"
#include "mysqlDb.h"
#include "timerTask.h"

void daemonize(void (*function)())
{
	int					i, fd0, fd1, fd2;
	pid_t				pid;
	struct rlimit		rl;
	struct sigaction	sa;

	umask(0);
	/*获取最大文件描述*/
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0){
		//std::cout<<"can't get file limit"<<std::endl;
		LOG_ERROR<<"can't get file limit\n";
	}
		
	/*失去控制终端*/
	if ((pid = fork()) < 0){
		//std::cout<<"fork error"<<std::endl;
		LOG_ERROR<<"fork error \n";
	}
	else if (pid != 0) {
		LOG_INFO<<"not daemon process:quit \n";
		exit(0);
	}
	setsid();
	/*禁止打开终端，同时忽略sighup信号*/
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0){
		//std::cout<<"can't ignore SIGHUP"<<std::endl;
		LOG_ERROR<<"can't ignore SIGHUP\n";
	}
		
	if ((pid = fork()) < 0){
		//std::cout<<"fork error"<<std::endl;
		LOG_ERROR<<"fork error \n";
	}
	else if (pid != 0) {
		LOG_INFO<<"not daemon process:second quit \n";
		exit(0);
	}
	if (chdir("/") < 0){
		//std::cout<<"can't change directory to /"<<std::endl;
		LOG_ERROR<<"can't change directory to /\n";
	}
	/*关闭打开的文件描述符*/
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 65536;
	for (i = 0; i < rl.rlim_max; i++)
		close(i);
	/*将标准输入输出重定向到/dev/null.*/
	fd0 = open("/dev/null", O_RDWR);
	//fd0 = open("/log.txt", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);
	function();
}
void serverInit()
{
	/*2000秒更新缓存*/
	TimerTask::addTimerTask(std::bind(&Cache::callback),200);
	/*20秒写一次日志*/
	TimerTask::addTimerTask(std::bind(&Log::writeLog,&Log::getLog()),1);
	TimerTask::start();
	
	ThreadPool pool;
	Dispatch dispatch(&pool);
	dispatch.runDispatch();
}
int main() 
{
	daemonize(serverInit);

	//serverInit();
	
	return 0;
}