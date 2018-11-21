#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <cstdlib>

using namespace std;
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
	
	if (bind(listenfd, &serveraddr, sizeof(serveraddr)) < 0)
		return -1;
	if (listen(listenfd, LISTENQ) < 0)
		return -1;
	return listenfd;
}



string  pages = "<html><head>
	<title>top news</title>
	</head><body>
	<p>Welcome to husky's top news.<br />
	</p>
	<hr>
	</body></html>" ;
void HomePage(int fd) 
{
	string reply;     
    reply += "HTTP/1.0 200 OK\r\n";    
    reply += "Server: Top news Web Server\r\n";
    reply += "Content-length:   "+ str(pages.size())  + " \r\n";
    reply += "Content-Type: text/html\r\n\r\n");
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
int main() 
{
	int listenfd, connfd;
	int clientlen;
	struct sockaddr_in clientaddr;
	listenfd = BindListen(80);
	/*目前是单核cpu，以后考虑用线程池*/
	while (1) {
		clientlen = sizeof(clientaddr);
		connfd = accept(listenfd, &clientaddr, &clientlen); 
		process(connfd);                                             
		close(connfd);                                           
	}
}