#ifndef _CACHE_H_
#define _CACHE_H_
//先暂时放到这里，后续得改
#include "mysqlDb.h"

enum EKEYTYPE{
	EDEFAULT = 0,
	EIMG,
	EARTICLE,
	EPAGE,
};
class Lru{
private:
	static std::unordered_map<std::string, OrmTable>newsmap;
	
	static int cachenum;
	static std::mutex lrumtx;
	static int curid;
	
public:
	static std::string getSection1();
	static std::string getSection2Page(int key);
	static std::string getSection2Article(std::string &skey);
	static std::string getSection3();
	static std::string getSection4();
	static std::string getHeader(int len) ;
	static std::string getErr() ;
	static std::vector<std::string>getHtml(std::string &skey);
	static std::string getImg(std::string &skey);
	static std::string getFile(std::string &skey);
	static int decodeSkey(std::string &skey,EKEYTYPE &etype);
};
#endif




