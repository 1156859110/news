#ifndef _MYSQLDB_H_
#define _MYSQLDB_H_
 
#include <mysql/mysql.h>

class OrmTable{
public:
	std::string sid;
	int artlen;
	int imglen;
	int cnt;
	std::string stitle;
	bool bimg;
	std::string spubdate;
	char *particle;
	char *pimg;
	OrmTable();
	~OrmTable();
};


class MysqlDb
{ 
public:
	MysqlDb();
	MysqlDb(std::string sid);
	~MysqlDb();
	bool init();         
	std::string queryArticle();
	int queryTitle(std::unordered_map<std::string, OrmTable> &dbmap);
private:
	std::string skey;      
	MYSQL* mysql;
	MYSQL_ROW row;
	MYSQL_RES* result;                                           
};
  
#endif                                                           
 