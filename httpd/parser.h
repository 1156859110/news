
class HttpParser
{
	private:
		int contentlen;
		int contentval;
	    int fd;
	    int epollfd;
		string method;
		string verion;
		string file;
		string contenttype;
		char *buf;
		int curindex;
		int state;
		int readindex;
		int preindex;
		const int bufsize = 4096;
	public:
		HttpParser():fd = 0,readdata = 0,readindex = 0,curindex = 0,preindex = 0,state = REQUEST,
		buf(new char[BUF_SIZE])
		{
			  memset(buf, '\0', BUF_SIZE);
		};
		~HttpParser(){
			  delete[] buf;
		};
		int parseLine();
		int parseRequest();
		int parseHeaders();
		int parseContent();
		int parseStart();
      int parser(); 
};













