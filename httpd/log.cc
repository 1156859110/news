
#include "common.h"
#include "log.h"

int Log::level = INFO;
static const char digits[] = "9876543210123456789";
static const char *zero = digits + 9;
template <typename T>
int Log::intTostr(char *pchar, T val) {
	char *pend = pchar;
	int i = 0;
	do{
		i = val % 10;
		*pend++ = zero[i];
		val /= 10;
	}while (val);
	
	if(val < 0)
		*pend++ = '-';
	*pend = '\0';
	
	std::reverse(pchar, pend);
	return pend - pchar;
}
template <typename T>
void Log::appendInteger(T i)
{
    char temp[64];
    int len = intTostr(temp, i);
    append(temp, len);
}
Log& Log::operator<<(unsigned int i)
{
    appendInteger(i);
    return *this;
}
Log& Log::operator<<(int i)
{
    appendInteger(i);
    return *this;
}
Log& Log::operator<<(unsigned long i)
{
    appendInteger(i);
    return *this;
}
Log& Log::operator<<(long i)
{
    appendInteger(i);
    return *this;
}
Log& Log::operator<<(long long i)
{
    appendInteger(i);
    return *this;
}
Log& Log::operator<<(unsigned long long i)
{
    appendInteger(i);
    return *this;
}
Log& Log::operator<<(unsigned short i)
{
    appendInteger(i);
    return *this;
}
Log& Log::operator<<(short i)
{
    appendInteger(i);
    return *this;
}
Log& Log::operator<<(bool b) {
    append(b ? "1" : "0", 1);
    return *this;
}
Log& Log::operator<<(const char c)
{
    append(&c, 1);
    return *this;
}
Log& Log::operator<<(const char *str)
{
    append(str, strlen(str));
    return *this;
}

Log& Log::operator<<(const std::string& str)
{
    append(str.c_str(), str.size());
    return *this;
}

long Log::getTimeVal(){
	struct timeval time;
	gettimeofday(&time, NULL);
	//取10的10次方，大概几个小时的精度
	return((long)(time.tv_sec) * 1000000 + time.tv_usec)%10000000000;
}
void Log::getDate(char *pdate){
	
	//20181208
    time_t now = time(0);
    tm *ltm = localtime(&now);
 
    ltm->tm_year += 1900;
    ltm->tm_mon += 1;
	int iDate = ltm->tm_year*10000 + ltm->tm_mon*100 + ltm->tm_mday;
	char date[10];
	intTostr(date, iDate);
	strcpy(pdate,date);
}
//注意不线程安全的，在多线程里面调用之前先调用避免静态类构造问题
Log& Log::getLog(){
	static Log log;
	return log;
}

int Log::swapBuff(){
	char *ptemp;
	std::lock_guard<std::mutex> locker(mLock);
	if(findex >0 && bindex == 0){
		ptemp  = pf;
		pf = pb;
		pb = ptemp;
		bindex = findex;
		findex = 0;
	}
	else if(findex == 0 && bindex > 0){
		ptemp  = pf;
		pf = pb;
		pb = ptemp;
	}	
	return 0;
}
void Log::append(const char *pchar, int len)
{
    if(len + findex > bufsize){
    	++losnum;
    }
    else{
		std::lock_guard<std::mutex> locker(mLock);
    	if(losnum > 0){
			std::string str = "lost log nums "+std::to_string(losnum);
			int size = str.size();
			if(size + findex > bufsize) {
				++losnum;
				return;
			}
			 memcpy(pf+findex,str.c_str(),size);
			 losnum = 0;
			 findex += size;
    	}
	    if(len + findex > bufsize){
	    	++losnum;
			return;
	    }
		memcpy(pf+findex, pchar, len);
		findex += len;
    }
}


void Log::writeLog()
{
	FILE * pFile;
	char filename[10];
	getDate(filename);
	{
		swapBuff();
		if(bindex == 0) return;
		pFile = fopen(filename, "a+");
		fwrite (pb , sizeof(char), bindex, pFile);
		fclose (pFile);
		memset(pb,0,sizeof(char)*bufsize);
		bindex = 0;
	}
}
