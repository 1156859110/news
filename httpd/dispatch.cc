#include "common.h"

#include "log.h"
#include "epoll.h"
#include "timer.h"
#include "parser.h"
#include "eventThread.h"
#include "threadPool.h"
#include "dispatch.h"

const int LISTENQ = 1024;

int Dispatch::setNonBlocking(int sockfd){
    int opts = 0;
	int rc = 0;
    opts = fcntl(sockfd, F_GETFL);
    if(opts < 0) {
    	 LOG_ERROR<<"fcntl get error\n";
		 return opts;
    }
    opts = (opts | O_NONBLOCK);
    if((rc = fcntl(sockfd, F_SETFL, opts)) < 0) {
    	 LOG_ERROR<<"fcntl set error\n";
    }
	return rc;
}

int Dispatch::bindListen(){
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
	std::cout<<"listenfd is "<<listenfd<<std::endl;
	LOG_INFO<<"listenfd is "<<listenfd;
	return 0;
}
Dispatch::Dispatch(ThreadPool* pool):port(80),pthrdpool(pool){
	bindListen();
	setNonBlocking(listenfd);   
	pepoll = new Epoll(listenfd);    
}
Dispatch::~Dispatch(){ 
	close(listenfd);  
	delete pepoll; 
}
void Dispatch::runDispatch(){
	//Dispatch *pdispatch = (Dispatch *)arg;
	//注意此处不初始化会导致accept异常。
	socklen_t clientlen = 0;
	struct sockaddr_in clientaddr;
	EventThread *pevthrd = NULL;
	int connfd = 0;
	std::cout<<"run dispatch "<<pepoll<<std::endl;
	 while(true){
		 pepoll->epollWait(-1);
		  //std::cout << "new thread pool " << pthrdpool<< std::endl;
		 while((connfd=accept(listenfd,(struct sockaddr*)&clientaddr, &clientlen)) > 0){
			 std::cout << "new connection fd " << connfd<< std::endl;
			 LOG_DEBUG << "new connection fd " <<connfd<<"\n";
			 setNonBlocking(connfd);
			 pevthrd =  pthrdpool->getEventThread();
			 pevthrd->addConList(connfd);
		 }	
		
		pthrdpool->notify();
	 }                              
}