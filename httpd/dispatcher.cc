#include "common.h"
#include "thread_poll.h"
#include "log.h"

using namespace std;

const int LISTENQ = 1024;

Dispatcher::Dispatcher() 
{
	bindListen(80);
	setNonBlocking(listenfd);
	pepoll = new Epoll(listenfd);                   
}
Dispatcher::~Dispatcher() 
{ 
	delete pepoll;
	close(listenfd);         
}
int Dispatcher::setNonBlocking(int sockfd) 
{
    int opts;
    opts = fcntl(sockfd, F_GETFL);
    if(opts < 0) {
    	 LOG_ERROR<<"fcntl get error\n";
    }
    opts = (opts | O_NONBLOCK);
    if(fcntl(sockfd, F_SETFL, opts) < 0) {
    	 LOG_ERROR<<"fcntl set error\n";
    }
}

int Dispatcher::bindListen() 
{
	struct sockaddr_in serveraddr;
  
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		LOG_ERROR<<"socket set error\n";
		exit(1);
	}
	/*timewait状态立马复用*/
	//if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int)) < 0)
	//	return -1;
	
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family      = AF_INET; 
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	serveraddr.sin_port        = htons((unsigned short)port); 
	
	if (bind(listenfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
		LOG_ERROR<<"bind error\n";
		exit(1);
	}
	if (listen(listenfd, LISTENQ) < 0){
		LOG_ERROR<<"listen error\n";
		exit(1);
	}
	cout<<"listenfd is "<<listenfd<<endl;
	LOG_INFO<<"listenfd is "<<listenfd;
	return 0;
}
void Dispatcher::run() 
{
	//注意此处不初始化会导致accept异常。
	socklen_t clientlen = 0;
	struct sockaddr_in clientaddr;
	int connfd = 0;
	 while(true){
		 pepoll->epollWait();
		 while((connfd=accept(listenfd,(struct sockaddr*)&clientaddr, &clientlen)) > 0){
			 cout << "new connection fd " << connfd<< endl;
			 LOG_DEBUG << "new connection fd " <<connfd;
			 setNonBlocking(connfd);
			  //分发事件
			 pthread->getThread(connfd);
		 }		
	 }                              
}