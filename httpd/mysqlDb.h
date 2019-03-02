#ifndef _MYSQLDB_H_
#define _MYSQLDB_H_
 
#include <mysql/mysql.h>

class OrmTable{
public:
	std::string sid;
	int cnt;
	std::string stitle;
	bool bimg;
	std::string spubdate;
	std::string sarticle;
	std::string simg;
	OrmTable():sid(""),cnt(0),stitle(""),bimg(false),spubdate(""),sarticle(""),simg(""){};
	~OrmTable(){};
};


class MysqlDb
{ 
public:
	MysqlDb();
	MysqlDb(std::string sid);
	~MysqlDb();
	bool init();         
	std::string queryArticle();
	std::unordered_map<std::string, OrmTable>queryTitle();
private:
	std::string skey;      
	MYSQL* mysql;
	MYSQL_ROW row;
	MYSQL_RES* result;                                           
};
  
#endif                                                           
 