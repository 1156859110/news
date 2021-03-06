#ifndef _PARSER_H_

#define _PARSER_H_

class Parser
{
public:
	Parser(int ifd);
	~Parser();
	int parseLine();
	int parseReqline(char *pbuf);
	int parseHeaders(char *pbuf);
	int parseContent();
	int parseStart();
	bool readRequest();
	int getResponse();
	bool sendResponse();
private:
	int fd;
	std::string method;
	std::string version;
	std::string skey;
	std::string contenttype;
	int contentlen;
	int curindex;
	int readindex;
	int preindex;
	int state;
	char *readbuf;
	char *writebuf;
	int wbufsize;
	int writeindex;
	std::list<std::string>sendlist;
};

#endif