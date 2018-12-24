#ifndef _PARSER_H_

#define _PARSER_H_

class Parser
{
	private:
	    int fd;
		std::string method;
		std::string version;
		int key;
		std::string contenttype;
		int contentlen;
		int curindex;
		int readindex;
		int preindex;
		int state;
		char *readbuf;
		char *writebuf;
		char wbufsize;
		int writeindex;
		std::list<std::pair<char *,int>>sendlist;
	public:
		Parser();
		~Parser();
		int parseLine();
		int parseReqline(char *pbuf);
		int parseHeaders(char *pbuf);
		int parseContent();
		int parseStart();
      int parser(); 
	  
	  void readRequest();
	  int getResponse();
	  bool sendResponse();
};

#endif











