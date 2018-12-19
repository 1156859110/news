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

enum { OK = 0, ERROR ,UNFINISHED,FINISHED,REQUEST, HEADER, CONTENT};	
const int BUFSIZE = 1024;
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


int Parser::parseLine()
{
    char temp;
    for ( ; curindex < readindex; ++curindex)
    {
        temp = buf[curindex];
        if (temp == '\r')
        {
            if ((curindex + 1 ) == readindex )
            {
                return UNFINISHED;
            }
            else if (buf[curindex + 1] == '\n' )
            {
                buf[curindex++] = '\0';
                buf[curindex++] = '\0';
                return OK;
            }
            return ERROR;
        }
        else if( temp == '\n')
        {
            if( ( curindex > 1 ) &&  buf[curindex - 1] == '\r' )
            {
                buf[curindex-1 ] = '\0';
                buf[curindex++] = '\0';
                return OK;
            }
            return ERROR;
        }
    }
    return UNFINISHED;
}

int Parser::parseRequest()
{
    if (strcasecmp(pbuf, "GET" ) == 0 )
    {
		method = "GET";
		pbuf += 4;
        std::cout<<"method is GET"<<std::endl;
    }
    else if(strcasecmp(pbuf, "POST" ) == 0)
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

int Parser::parseHeaders()
{
	//碰到头部是空行的说明解析头部已经结束
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
int Parser::parseContent()
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
	std::cout<<"content len "<< contentlen<std::endl;
    return FINISHED;
}
int Parser::parseStart()
{
    int rc = OK;
    while((rc = parseLine(buf, curindex, readindex)) == OK )
    {
        char* pbuf = buf + preindex;
        preindex = curindex;
        switch (state)
        {
            case REQUEST:
            {
                if ((rc = parseRequest())== ERROR)
                {
                    return ERROR;
                }
				state = HEADER;
                break;
            }
            case HEADER:
            {
               rc = parseHeaders();
				state = (rc == OK?CONTENT:HEADER);
                break;
            }
			case CONTENT:
			{
				rc = parseContent();
				break;
			}
            default:
                rc = ERROR;
        }
    }
	
   return rc;
}

int Parser::readRequest()
{
   readdata = read(fd, readbuf + readindex, readbuf.size() - readindex)
   readindex += readdata;
	 
    parseStart(readbuf, curindex, state, readindex, preindex);
	
   
    return 0;
}
int Parser::getResponse()
{
	//需要持续写入，加入到epoll里面
    writedata = write(fd, writebuf + writeindex, writebuf - writeindex);
    writeindex += writedata;
    return 0;
}
int Parser::sendResponse()
{
	//需要持续写入，加入到epoll里面
    writedata = write(fd, writebuf + writeindex, writebuf - writeindex);
    writeindex += writedata;
    return 0;
}









