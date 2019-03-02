#ifndef _CACHE_H_
#define _CACHE_H_
class OrmTable;
enum EKEYTYPE{
	EDEFAULT = 0,
	EIMG,
	EARTICLE,
	EPAGE,
};
class Cache{
private:
	static std::unordered_map<std::string, OrmTable>newsmap;
	static int cachenum;
	static std::mutex mtx;
	//static pthread_rwlock_t rwlock;
	static int curid;
	static int preid;
	
public:
	static int getSection1(std::string &s1);
	static int getSection2Page(int key,std::string &s2);
	static int getSection2Article(int key,std::string &s2);
	static int getSection3(std::string &s3);
	static int getSection4(std::string &s4);
	static std::string getHeader(int len);
	static std::string getErr();
	static std::vector<std::string>getHtml(std::string &skey);
	static std::string getImages(std::string &skey);
	static int decodeSkey(std::string &skey,EKEYTYPE &etype);
	static void updateCache();
	static void callback();
	
};
#endif




