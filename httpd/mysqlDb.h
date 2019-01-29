#ifndef _MYSQLDB_H_
#define _MYSQLDB_H_
 
#include <mysql/mysql.h>

typedef struct {
	int id;
	int artlen;
	int imglen;
	int cnt;
	char title[50];
	bool bimg;
	char pubdate[10];
	char *particle;
	char *pimg;
}Sdbtable;


class MysqlDb
{ 
public:
	MysqlDb();
	MysqlDb(int curid);
	~MysqlDb();
	bool init();         
	std::string queryArticle();
	std::unordered_map<std::string, Sdbtable>  queryTitle();
private:
	unsigned int key;      
	MYSQL* mysql;
	MYSQL_ROW row;
	MYSQL_RES* result;                                           
};
  
#endif                                                           
 