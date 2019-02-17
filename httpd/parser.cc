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
#include "log.h"
#include "cache.h"
#include "parser.h"

enum { OK = 0, ERR ,UNFINISHED,FINISHED,REQUEST, HEADER, CONTENT};	
const int BUFSIZE = 1024;

Parser::Parser(int ifd):fd(ifd),writeindex(0),wbufsize(0),readindex (0),curindex (0),preindex (0),
state(REQUEST),readbuf(new char[BUFSIZE+1]),writebuf(NULL),contentlen(0),method("UNKNOWN"),
version("UNKNOWN"),skey(""),contenttype("UNKNOWN"){
	memset(readbuf,0,sizeof(BUFSIZE));
};

Parser::~Parser(){
	delete[] readbuf;
}

int Parser::parseLine(){
	for ( ; curindex < readindex; ++curindex){
		if((readbuf[curindex] == '\n') && (curindex >= 1 ) && readbuf[curindex - 1] == '\r'){
			++curindex;
			return OK;
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
	//需要清除上次的skey
	skey = "";
	pbuf += 2;
	while(pbuf[0] != ' '){
		skey += pbuf[0];
		++pbuf;
	}
	if(skey == "") skey = "page1";
	std::cout<<"skey is"<<skey<<std::endl;
	++pbuf;//指向httpversion
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
		while(pbuf[i] != '\r'){
			if(pbuf[i] == ' ') continue;
			contentlen = contentlen*10+ pbuf[i]-'0';
			++i;
		}
    }
    return UNFINISHED;
}
int Parser::parseContent(){
	std::cout<<"content len "<< contentlen<<std::endl;
	LOG_DEBUG<<"content len "<< contentlen;

	if(curindex + contentlen > readindex){
		contentlen -= readindex-curindex;
		curindex = readindex;
	} else{
		curindex += contentlen;
		contentlen = 0;
		state = FINISHED;
	}
	preindex = curindex;
    return 0;
}
int Parser::parseStart(){
	char* pbuf = NULL;
	std::cout<<readindex<<"read index "<<std::endl;
	while(curindex < readindex){
		while(state == REQUEST || state == HEADER){
			if(parseLine() != OK) return -1;
			std::cout<<curindex<<"cur index "<<std::endl;
			pbuf     = readbuf + preindex;
			preindex = curindex;
			switch (state){
				case REQUEST:
					parseReqline(pbuf);
					state   = HEADER;
					break;
				case HEADER:		           
					if(parseHeaders(pbuf) == FINISHED){
						state  = CONTENT;
					}
					break;
			}
		}
		//现在pre和cur是相等的,都指向内容行。
		pbuf = readbuf + preindex;
		//暂时不处理content内容
		parseContent();
		if(state == FINISHED){
			getResponse();
			std::cout<<curindex<<" "<<readindex<<"curindex  readindex "<<std::endl;
			state     = REQUEST;
		}
		
	}
	return 0;
}

bool Parser::readRequest(){
	if(readindex == BUFSIZE){
		memmove(readbuf,readbuf+preindex,BUFSIZE - preindex);
		memset(readbuf + BUFSIZE - preindex,0,preindex);
		readindex -= preindex;
		curindex -= preindex;
		preindex = 0;
	}
   int readdata = read(fd, readbuf + readindex,  BUFSIZE - readindex);
   //返回为0，说明连接关闭。
   if(readdata <= 0 ) {
	   std::cout<<readdata<<" read data error "<<std::endl;
	   return false;
   }
   readindex += readdata;
   //需要多开辟一个字节
   readbuf[readindex] = '\0';
   std::cout<<readdata<<" data read to buff \n"<<readbuf + readindex - readdata<<std::endl;
   parseStart();
   return true;
}
int Parser::getResponse(){
	//本线程不会对list同时进行添加或者删除操作，不需要锁
	//std::string sendbufs = Lru::getHtml(skey);
	std::vector<std::string>  v = Lru::getHtml(skey);
	skey = "";
	for(auto &it:v){
		sendlist.push_back(it);
	}
	
	//std::cout<<psend<<" data"<<std::endl;
	//std::cout<<size<<" ptr size"<<std::endl;
    return 0;
}
bool Parser::sendResponse(){
	bool bcontinue = false;
	do{
		std::cout <<"list size is "<<sendlist.size()<<std::endl;
		if(sendlist.empty()) return true;
		std::string sendbufs = sendlist.front();
		wbufsize = sendbufs.size();
		std::cout<<" fd " <<fd<<" buf size " <<wbufsize<<" write index"<<writeindex<<std::endl;
	    int writedata = write(fd, sendbufs.c_str() + writeindex,wbufsize - writeindex);
		if(writedata < 0) return false;
	    writeindex += writedata;
		std::cout<<fd <<" wite fd"<<writedata<<" write data" <<std::endl;
		bcontinue  = (writeindex == wbufsize)?true:false;
		if(writeindex == wbufsize){
			writeindex = 0;
			wbufsize = 0;
			sendlist.pop_front();
		}
	}while(bcontinue);
	
	return writeindex == wbufsize;
}









