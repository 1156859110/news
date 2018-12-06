/*GET / HTTP/1.1                    .............请求行  
Host: middleouts.com                .............请求头部
Connection: keep-alive              .............
Cache-Control: max-age=0            .............
Upgrade-Insecure-Requests: 1       ..............
回车 换行                           ............头部结束
请求数据                            ............请求数据 
GET /form.html HTTP/1.1 (CRLF)
Accept:image/gif,image/x-xbitmap,image/jpeg,application/x-shockwave-flash,application/vnd.ms-excel,application/vnd.ms-powerpoint,application/msword,*/* (CRLF)
Accept-Language:zh-cn (CRLF)
Accept-Encoding:gzip,deflate (CRLF)
If-Modified-Since:Wed,05 Jan 2007 11:21:25 GMT (CRLF)
If-None-Match:W/"80b1a4c018f3c41:8317" (CRLF)
User-Agent:Mozilla/4.0(compatible;MSIE6.0;Windows NT 5.0) (CRLF)
Host:www.guet.edu.cn (CRLF)
Connection:Keep-Alive (CRLF)
(CRLF)
	*/
/*	
	POST /reg.jsp HTTP/ (CRLF)
	Accept:image/gif,image/x-xbit,... (CRLF)
	...
	HOST:www.guet.edu.cn (CRLF)
	Content-Length:22 (CRLF)
	Connection:Keep-Alive (CRLF)
	Cache-Control:no-cache (CRLF)
	(CRLF)         //该CRLF表示消息报头已经结束，在此之前为消息报头
	user=jeffrey&pwd=1234  //此行以下为提交的数据
*/
	
class HttpParser
{
	private:
		int bodylen;
		int bodyvalue;
	    int fd;
	    int epollfd;
		string method;
		string verion;
		string file;
		string contenttype;
	public:	
		
   
};

string  pages = "<html><head>\
	<title>husky</title>\
	</head><body>\
	<div align=\"center\">\
		Welcome to husky's home page!\
	</div>\
	</body></html>";
void HomePage(int fd) 
{
	string reply;     
    reply += "HTTP/1.1 200 OK\r\n";    
    reply += "Server: husk's Web Server\r\n";
    reply += "Content-length:   "+ to_string(pages.size())  + " \r\n";
    reply += "Content-Type: text/html\r\n\r\n";
	reply += "Connection: Keep-Alive\r\n";
	reply += pages;
	
    write(fd, reply.c_str(), strlen(reply.c_str()));                      
 
   return;                 
}
void notFind(int fd) 
{
	string reply;     
    reply += "HTTP/1.1 400 OK\r\n";    
    reply += "Server: husk's Web Server\r\n";
    reply += "Content-length:   "+ to_string(pages.size())  + " \r\n";
    reply += "Content-Type: text/html\r\n\r\n";
	reply += pages;
	
    write(fd, reply.c_str(), strlen(reply.c_str()));                      
        
   return;                 
}


httpParser(const char* cbuff)
{
	string sbuff = cbuff;
	int pre = 0;
	int cur = 0;
	
	pre = cur;
	cur = sbuff.find(' ',cur);
	if(cur < 0) return false;
	method = sbuff.substr(pre,cur);
	
	pre = cur;
	cur = sbuff.find(' ',cur);
	if(cur < 0) return false;
	file = sbuff.substr(pre,cur);
	
	pre = cur;
	cur = sbuff.find(' ',cur);
	if(cur < 0) return false;
	version = sbuff.substr(pre,cur);

	pre = cur;
	cur = sbuff.find('Content-Length:',pre);
	if(cur != -1){
		pre = cur;
		cur = sbuff.find('\r\n',cur);
		bodylen = stoi(sbuff.substr(pre,cur));
		cur = sbuff.find('\r\n\r\n',0);
		bodyvalue = stoi(sbuff.substr(cur,cur+bodylen));
	}
	cur = sbuff.find('Content-Type:',0);
	if(cur != -1){
		pre = cur;
		cur = sbuff.find('\r\n',cur);
		contenttype = stoi(sbuff.substr(pre,cur));
	}
}
bool checkRequst()
{
	if(method != "GET" || method != "POST")
		return false;
	
	if(file == "") return false;
	
	if(version != "HTTP/1.1" || version != "HTTP/1.0")
		
		return false;
}

/*需要自己解析多个报文。有可能出现收到的不是一个完整的包的情况，用自动机或者buff处理。*/














