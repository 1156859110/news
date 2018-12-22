class Parser
{
	private:
	    int fd;
		string method;
		string verion;
		string skey;
		string contenttype;
		int contentlen;
		int curindex;
		int readindex;
		int preindex;
		int state;
		char *readbuf;
		char *writebuf;
		char wbufsize;
		int writeindex;
		list<parit<char *,int>>sendlist;
	public:
		Parser();
		~Parser();
		int parseLine();
		int parseRequest();
		int parseHeaders();
		int parseContent();
		int parseStart();
      int parser(); 
};













