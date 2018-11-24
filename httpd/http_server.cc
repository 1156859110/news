#include <iostream>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <cstdlib>
#include<fcntl.h>
#include<sys/stat.h> 
#include<signal.h>
#include <sys/resource.h>
using namespace std;

const int LISTENQ = 1024;
int BindListen(int port) 
{
	int listenfd , optval = 1;
	struct sockaddr_in serveraddr;
  
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1;
	/*timewait状态立马复用*/
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int)) < 0)
		return -1;
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family      = AF_INET; 
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	serveraddr.sin_port        = htons((unsigned short)port); 
	
	if (bind(listenfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
		return -1;
	if (listen(listenfd, LISTENQ) < 0)
		return -1;
	return listenfd;
}



string  pages = "<html><head>\
	<title>top news</title>\
	</head><body>\
	<div align=\"center\">\
		Welcome to husky's home page！\
	</div>\
	</body></html>";
void HomePage(int fd) 
{
	string reply;     
    reply += "HTTP/1.0 200 OK\r\n";    
    reply += "Server: Top news Web Server\r\n";
    reply += "Content-length:   "+ to_string(pages.size())  + " \r\n";
    reply += "Content-Type: text/html\r\n\r\n";
	reply += pages;
	
    write(fd, reply.c_str(), strlen(reply.c_str()));                      
        
   return;                 
}
void process(int fd) 
{
	char buf[9600]={0};
	read(fd,buf,9600);
	HomePage(fd);
}
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
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);
#if 0
	/* * Initialize the log file. */
	openlog(cmd, LOG_CONS, LOG_DAEMON);
	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		syslog(LOG_ERR, "unexpected file descriptors %d %d %d",
		  fd0, fd1, fd2);
		exit(1);
	}
#endif
	
	pFunction();
}
void server() 
{
	int listenfd, connfd;
	int clientlen;
	struct sockaddr_in clientaddr;
	listenfd = BindListen(80);
	/*目前是单核cpu，以后考虑用线程池*/
	while (1) {
		clientlen = sizeof(clientaddr);
		connfd = accept(listenfd,(struct sockaddr *)&clientaddr, (socklen_t *)&clientlen); 
		process(connfd);                                             
		close(connfd);                                           
	}
}
int main() 
{
	daemonize(server);
}