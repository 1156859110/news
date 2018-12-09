/*GET / HTTP/1.1                    .............请求行  
Host: middleouts.com                .............请求头部
Connection: keep-alive              .............
Cache-Control: max-age=0            .............
Upgrade-Insecure-Requests: 1       ..............
回车 换行                           ............头部结束
请求数据                            ............请求数据 
POST /reg.jsp HTTP/1.1 (CRLF)
Accept:image/gif,image/x-xbit,... (CRLF)
...
HOST:www.guet.edu.cn (CRLF)
Content-Length:22 (CRLF)
Connection:Keep-Alive (CRLF)
Cache-Control:no-cache (CRLF)
(CRLF)         //该CRLF表示消息报头已经结束，在此之前为消息报头
user=jeffrey&pwd=1234  //此行为提交的数据
*/

#define BUF_SIZE 9600
enum LINE_STATUS { OK = 0, ERROR, UNFINISHED,METHOD, HEADER, CONTENT,FINISHED};	

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


int parseLine(char* buffer, int& curindex, int& readindex)
{
    char temp;
    for ( ; curindex < readindex; ++curindex)
    {
        temp = buffer[curindex];
        if (temp == '\r')
        {
            if ((curindex + 1 ) == readindex )
            {
                return UNFINISHED;
            }
            else if (buffer[curindex + 1] == '\n' )
            {
                buffer[curindex++] = '\0';
                buffer[curindex++] = '\0';
                return OK;
            }
            return ERROR;
        }
        else if( temp == '\n')
        {
            if( ( curindex > 1 ) &&  buffer[curindex - 1] == '\r' )
            {
                buffer[curindex-1 ] = '\0';
                buffer[curindex++] = '\0';
                return OK;
            }
            return ERROR;
        }
    }
    return UNFINISHED;
}

int parseRequest(char* pbuf)
{
    if (strcasecmp(pbuf, "GET" ) == 0 )
    {
		method = "GET";
		pbuf += 4;
        std::cout<<"method is GET"<<std::endl;
    }
    else  if(strcasecmp(pbuf, "POST" ) == 0)
    {
		method = "POST";
		pbuf += 5;
		std::cout<<"method is POST"<<std::endl;
    }
    else
    {
		std::cout<<"unknown method"<<std::endl;
        return ERROR;
    }
	char *p = strchr(pbuf,' ');
	if(p == NULL) return ERROR;
	pbuf = p + 1;//指向httpversion
    if (strcasecmp(pbuf, "HTTP/1.1" ) == 0 ){
    	version = "HTTP/1.1";
    }
	else if(strcasecmp(pbuf, "HTTP/1.0" ) == 0 ){
		version = "HTTP/1.0";
	}
	else{
		std::cout<<"unknown version"<<std::endl;
		return ERROR;
	}
    return OK;
}

int parseHeaders( char* pbuf )
{
    if (pbuf[0] == '\r')
    {
        return OK;
    }
	
    if (strncasecmp(pbuf, "Content-Length:", 15 ) == 0)
    {
        pbuf += 15;
		int i = 0;
		while(pbuf[i]!=' '){
			contentlen = contentlen*10+pbuf[i];
			i++;
		}
    }
  
    return UNFINISHED;
}
int parseContent(char* pbuf)
{
	std::cout<<"content len "<< contentlen<std::endl;
    if (contentlen == 0)
    {
        return FINISHED;
    }
	int i = 0;
	while(i < contentlen){
		contentval = contentval*10+pbuf[i];
		++i;
	}
    return FINISHED;
}
int parseStart( char* buffer, int& curindex, int& state, int& readindex, int& preindex )
{
    int rc = OK;
    while((rc = parseLine(buffer, curindex, readindex)) == OK )
    {
        char* pbuf = buffer + preindex;
        preindex = curindex;
        switch (state)
        {
            case METHOD:
            {
                if ((rc = parseMethod(pbuf))== ERROR)
                {
                    return ERROR;
                }
				state = HEADER;
                break;
            }
            case HEADER:
            {
               rc = parseHeaders(pbuf)；
				state = rc == OK?CONTENT:HEADER;
                break;
            }
			case CONTENT:
			{
				return parseContent(pbuf);
			}
            default:
                return ERROR;
        }
    }
	
   return rc;
}

int httpParser(int fd)
{
      
        memset(buffer, '\0', BUF_SIZE);
        int readdata = 0;
        int readindex = 0;
        int curindex = 0;
        int preindex = 0;
        int state = METHOD;
        while(1)
        {
            readdata = read(fd, buffer + readindex, BUF_SIZE - readindex);
            if (readdata == -1)
            {
                std::cout<<"read error"<<std::endl;
                break;
            }
            else if (readdata == 0)
            {
                std::cout<<"remote client closed the connection"<<std::endl;
                break;
            }
            readindex += readdata;
            int rc = parseStart(buffer, curindex, state, readindex, preindex);
            if(rc == NO_REQUEST)
            {
                continue;
            }
            else
            {
                //sendResponse( fd, "helloword", 9, 0 );//ok
                break;
			}
        }
    return 0;
}









