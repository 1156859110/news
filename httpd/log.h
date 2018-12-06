
#define LOG_DEBUG if(Log.level >= Log::DEBUG)\
    Log.getLog()<<__FILE__<<__LINE__<<getTimeVal();
#define LOG_INFO  if(Log.level >= Log::INFO)\
    Log.getLog()<<__FILE__<<__LINE__<<getTimeVal();
#define LOG_ERROR if(Log.level >= Log::ERROR)\
    Log.getLog()<<__FILE__<<__LINE__<<getTimeVal();
	
class Log{
private:
    template <typename T>;
    int intTostr(char *pchar, T val)s
	void appendInteger(T val);
	const static int bufsize = 1024*1000;
	static char rbuf[bufsize];//将buff里面的数据写到文件
	static char wbuf[bufsize];//将数据写入到buff里面
	char *pw;
	char *pr;
	int losnum;//丢失的log条数
	int rindex;//数据写入到buff的index	
	int windex;
	std::mutex mLock;
	static int level;
    enum loglevel{
		DEBUG,
		INFO,
		ERROR,
    };
	
	
public:
   
    Log &operator<<(bool);

    Log &operator<<(unsigned char);
    Log &operator<<(const char);
    Log &operator<<(const char *);

    Log &operator<<(unsigned int);
    Log &operator<<(unsigned long);
    Log &operator<<(unsigned long long);
    Log &operator<<(unsigned short);
    Log &operator<<(long);
    Log &operator<<(int);
    Log &operator<<(long long);
    Log &operator<<(short);

    Log &operator<<(const std::string &);
	
	void Log::getLevel();
	
	static Log &getLog();
	
	 ~Log();
};

