
class Log{
private:
    template <typename T>;
    int intTostr(char *pchar, T val);
	void appendInteger(T val);
	const static int bufsize = 1024*1000;
	static char rbuf[bufsize];//将buff里面的数据写到文件
	static char wbuf[bufsize];//将数据写入到buff里面
	char *pw;
	char *pr;
	int losnum;//丢失的log条数
	int index;//数据写入到buff的index
	//锁
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
	
	 ~Log();
};

void Log::append(const char *pchar, int len)
{
    if(len + index > bufsize){
    	++losnum;
    }
    else{
    	if(losnum > 0){
			string str  = "lost log nums "+to_str(losnum);
			int size = str.size();
			if(size + index > bufsize) {
				++losnum;
				return;
			}
			
			 memcpy(&buf[index],str.c_str(),size);
			 losnum = 0;
			 index += size;
    	}
	    if(len + index > bufsize){
	    	++losnum;
			return;
	    }
		memcpy(&buf[index], pchar, len);
		index += len;
    }
}

static const char digits[] = "9876543210123456789";
static const char *zero = digits + 9;

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
void Log::appendInteger(T i)
{
    char temp[64];
    int len = intTostr(temp, i);
    buf.append(temp, len);
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
    buf.append(b ? "1" : "0", 1);
    return *this;
}
Log& Log::operator<<(const char c)
{
    buf.append(&c, 1);
    return *this;
}
Log& Log::operator<<(const char *str)
{
    buf.append(str, strlen(str));
    return *this;
}

Log& Log::operator<<(const std::string& str)
{
    buf.append(str.c_str(), str.size());
    return *this;
}

Log::~Log()
{
    //todo
}
















