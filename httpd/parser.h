
class Parser
{
	private:
		int contentlen;
		int contentval;
	    int fd;
		string method;
		string verion;
		string file;
		string contenttype;
		int curindex;
		int readindex;
		int preindex;
		int state;
		vector<char>readbuf;
		vector<char>writebuf;
	public:
		Parser():fd(0),readdata (0),readindex (0),curindex (0),preindex (0),state(REQUEST)){};
		~Parser(){}
		int parseLine();
		int parseRequest();
		int parseHeaders();
		int parseContent();
		int parseStart();
      int parser(); 
};













