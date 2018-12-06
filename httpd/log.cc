
void Log::append(const char *pchar, int len)
{
    if(len + index > bufsize){
    	++losnum;
    }
    else{
		std::lock_guard<std::mutex> locker(mLock);
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


long getTimeVal(){
	struct timeval time;
	gettimeofday(&time, NULL);
	return((long)(time.tv_sec) * 1000000 + time.tv_usec);
}
void getDate(char *pdate){
	
	//20181208
    time_t now = time(0);
    tm *ltm = localtime(&now);
 
    ltm->tm_year += 1900;
    ltm->tm_mon += 1;
	int iDate = ltm->tm_year*10000 + ltm->tm_mon + tm->tm_mday;
	char date[10];
	intTostr(date, iDate);
	strcpy(pdate,date);
}

static Log &getLog(){
	static Log log;//注意不线程安全的，后续改为线程安全的。
	return log;
}

int swapBuff(){
	std::lock_guard<std::mutex> locker(mLock);
	if(rindex >0 && windex == 0){
		pr = wbuf;//将buff里面的数据写到文件
		pw = rbuf;//将数据写入到buff里面
	}	
	if(rindex == 0 && windex > 0){
		pr = rbuf;
		pw = wbuf;
		
	}
		
}
void Log::writeLog()
{
	
	while (true){
		//每隔两秒写一次buff
		sleep(2);
		
		FILE * pFile;
		char filename[10];
		getDate(filename);
		
		{
			std::lock_guard<std::mutex> locker(mLock);
			pFile = fopen(filename, "a+");
			fwrite (buffer , sizeof(char), rindex, pFile);
			fclose (pFile);
		}
		
	}
}

 







