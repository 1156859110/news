#include "common.h"
#include "thread_poll.h"
#include "log.h"

using namespace std;

typedef void (*pf)(); 

void
daemonize(pf pFunction)
{
	int					i, fd0, fd1, fd2;
	pid_t				pid;
	struct rlimit		rl;
	struct sigaction	sa;

	umask(0);
	/*获取最大文件描述*/
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
		cout<<"can't get file limit"<<endl;
	/*失去控制终端*/
	if ((pid = fork()) < 0)
		cout<<"fork error"<<endl;
	else if (pid != 0) 
		exit(0);
	setsid();
	/*禁止打开终端，同时忽略sighup信号*/
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
		cout<<"can't ignore SIGHUP"<<endl;
	if ((pid = fork()) < 0)
		cout<<"fork error "<<endl;
	else if (pid != 0) 
		exit(0);

	if (chdir("/") < 0)
		cout<<"can't change directory to /"<<endl;
	/*关闭打开的文件描述符*/
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++)
		close(i);
	/*将标准输入输出重定向到/dev/null.*/
	//fd0 = open("/dev/null", O_RDWR);
	//fd1 = dup(0);
	//fd2 = dup(0);

	//ThreadPool();
	//pFunction();
}

int main() 
{
	//daemonize(server);
	ThreadPool();
	Log::setLevel(ERROR);
	Log::threadCreate();
	server();
}