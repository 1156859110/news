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

#include "common.h"
#include "parser.h"

enum { OK = 0, ERROR ,UNFINISHED,FINISHED,REQUEST, HEADER, CONTENT};	
const int BUFSIZE = 1024;

Parser::Parser():fd(0),readdata (0),readindex (0),curindex (0),preindex (0),
state(REQUEST),readbuf(new char[BUFSIZE]),writebuf(NULL)){
	memset(readbuf,0,sizeof(BUFSIZE));
};

Parser::~Parser(){
	delete[] readbuf;
}

int Parser::parseLine(){
    char temp;
    for ( ; curindex < readindex; ++curindex){
        temp = readbuf[curindex];
        if (temp == '\r'){
            if ((curindex + 1 ) == readindex ){
                return UNFINISHED;
            }
            else if (readbuf[curindex + 1] == '\n' ){
                return OK;
            }
            return ERROR;
        }
        else if( temp == '\n'){
            if( ( curindex > 1 ) &&  readbuf[curindex - 1] == '\r'){
                return OK;
            }
            return ERROR;
        }
    }
    return UNFINISHED;
}

int Parser::parseReqline(char *pbuf)
{
    if (strncasecmp(pbuf, "GET",3) == 0 ){
		method = "GET";
		pbuf += 3;
    }
    else if(strncasecmp(pbuf, "POST",4) == 0){
		method = "POST";
		pbuf += 4;
    }
    else if(strncasecmp(pbuf, "HEAD",4) == 0){
		method = "HEAD";
		pbuf += 4;
    }
	else{
		method = "UNKNOWN";
    }
	std::cout<<"method is"<<method<<std::endl;
	
	char *p = strchr(pbuf,' ');
	if(p == NULL) return ERROR;
	strncpy(skey.c_str(),pbuf,p-pbuf);
	std::cout<<"file is"<<skey<<std::endl;
	pbuf = p + 1;//指向httpversion
    if (strncasecmp(pbuf, "HTTP/1.1" ,8) == 0 ){
    	version = "HTTP/1.1";
    }
	else if(strncasecmp(pbuf, "HTTP/1.0",8 ) == 0 ){
		version = "HTTP/1.0";
	}
	else{
		version = "UNKNOWN";
	}
	std::cout<<"version is "<<version<<std::endl;
    return OK;
}

int Parser::parseHeaders(char *pbuf)
{
	//碰到头部是空行的说明解析头部已经结束
    if (pbuf[0] == '\r')  return FINISHED;
	
    if (strncasecmp(pbuf, "Content-Length:", 15 ) == 0){
        pbuf += 15;
		int i = 0;
		while(pbuf[i] != ' '){
			contentlen = contentlen*10+ pbuf[i]-'0';
			++i;
		}
    }
    return UNFINISHED;
}
int Parser::parseContent(){
	std::cout<<"content len "<< contentlen<std::endl;
	LOG_DEBUG<<"content len "<< contentlen;
	//暂时不处理content内容
	curindex += contentlen;
	contentlen = 0;
	if(curindex > readindex) return UNFINISHED;
	preindex = curindex;
	state = FINISHED;
	
    return FINISHED;
}
int Parser::parseStart(){
	char* pbuf = NULL;
	while(curindex < readindex){
		if(state == REQUEST || state == HEADER){
		    while(parseLine(readbuf, curindex, readindex)) == OK ){
		        pbuf = readbuf + preindex;
		        preindex = curindex;
		        switch (state){
		            case REQUEST:
		                parseReqline(pbuf);
						state = HEADER;
		                break;
		            case HEADER:		           
		               if(parseHeaders(pbuf) == FINISHED){
		               	state = CONTENT;
		               }
		                break;
		        }
				if(state == CONTENT) break;
			}
		}
		//现在pre和cur是相等的。
	    pbuf = readbuf + preindex;
		parseContent(pbuf);
		if(state == FINISHED){
			getResponse();
			state = REQUEST;
		}
	}
   return 0;
}

void Parser::readRequest(){
	if(readindex == BUFSIZE){
		memmove(readbuf,readbuf+preindex,BUFSIZE - preindex);
		memset(readbuf + BUFSIZE - preindex,0,preindex);
		readindex -= preindex;
		curindex -= preindex;
		preindex = 0;
	}
   int readdata = read(fd, readbuf + readindex,  BUFSIZE - readindex);
   if(readdata < 0 ) return ;
   readindex += readdata;
   parseStart(readbuf, curindex, state, readindex, preindex);
}
int Parser::getResponse(){
	//获取wtirebuf和size，这里需要shared ptr,避免数据拷贝；
	char *psend = NULL;
	int size = 0;
	//本线程不会对list同时进行添加或者删除操作，不需要锁
	Lru::getData(key,psend,size);
	sendlist.push_back({psend,size});
    return 0;
}
bool Parser::sendResponse(){
	bool bcontinue = false;
	do{
		if(sendlist.size() == 0) return true;
		char *psend = sendlist.front().first;
		wbufsize = sendlist.front().second;
	    int writedata = write(fd, psend + writeindex,wbufsize - writeindex);
	    writeindex += writedata;
		bcontinue  = (writeindex == wbufsize);
		if(writeindex == wbufsize){
			writeindex = 0;
			wbufsize = 0;
			sendlist.pop_front();
		}
	}while(bcontinue);
	
	return writeindex == wbufsize;
}









