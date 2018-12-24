#include "common.h"
#include "log.h"
#include "timer.h"
#include "epoll.h"
#include "parser.h"
#include "eventThread.h"
#include "threadPool.h"
#include "dispatch.h"

void daemonize(void (*function)())
{
	int					i, fd0, fd1, fd2;
	pid_t				pid;
	struct rlimit		rl;
	struct sigaction	sa;

	umask(0);
	/*获取最大文件描述*/
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
		std::cout<<"can't get file limit"<<std::endl;
	/*失去控制终端*/
	if ((pid = fork()) < 0)
		std::cout<<"fork error"<<std::endl;
	else if (pid != 0) 
		exit(0);
	setsid();
	/*禁止打开终端，同时忽略sighup信号*/
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
		std::cout<<"can't ignore SIGHUP"<<std::endl;
	if ((pid = fork()) < 0)
		std::cout<<"fork error "<<std::endl;
	else if (pid != 0) 
		exit(0);

	if (chdir("/") < 0)
		std::cout<<"can't change directory to /"<<std::endl;
	/*关闭打开的文件描述符*/
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++)
		close(i);
	/*将标准输入输出重定向到/dev/null.*/
	//fd0 = open("/dev/null", O_RDWR);
	//fd1 = dup(0);
	//fd2 = dup(0);
	function();
}
void serverInit()
{
	Log::setLevel(ERROR);
	Log::threadCreate();
	ThreadPool pool;
	Dispatch dispatch(&pool);
	dispatch.runDispatch();
}
int main() 
{
	//daemonize(serverInit());
	serverInit();
	return 0;
}