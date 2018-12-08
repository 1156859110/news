#ifndef _LOG_H
#define _LOG_H

#include "common.h"

    enum loglevel{
		DEBUG,
		INFO,
		ERROR,
    };
void* run(void *arg);
class Log{
private:
    template <typename T>
    int intTostr(char *pchar, T val);
	template <typename T>
	void appendInteger(T val);
	int bufsize;
	char *pf;
	char *pb;
	int losnum;//丢失的log条数
	int bindex;
	int findex;
	std::mutex mLock;
	
   	static int level;
	pthread_t tid;
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
	
	
	Log():
		bufsize(1024*1000),
		pf(new char[bufsize]),
		pb(new char[bufsize]),
		losnum(0),
		bindex(0),
		findex(0),
		tid(0)
	{
		pthread_create(&tid, NULL,run, (void*)&getLog());
	}
	static Log& getLog();
	static long getTimeVal();
	void getDate(char *pdate);
	int swapBuff();
	void append(const char *pchar, int len);
	
	void writeLog();
	static int getLevel(){ return level;}
	static int setLevel(int lev){
		level = lev;
	}
	 ~Log();
};
#define LOG_DEBUG if(Log::getLevel() >= DEBUG)\
    Log::getLog()<<__FILE__<<__LINE__<<Log::getTimeVal()
#define LOG_INFO  if(Log::getLevel() >= INFO)\
    Log::getLog()<<__FILE__<<__LINE__<<Log::getTimeVal()
#define LOG_ERROR if(Log::getLevel() >= ERROR)\
    Log::getLog()<<__FILE__<<__LINE__<<Log::getTimeVal()

#endif