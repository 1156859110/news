#ifndef _MYSQLDB_H_
#define _MYSQLDB_H_
 
#include <mysql/mysql.h>

typedef struct {
	int id;
	char title[50];
	bool bimg;
	char pubdate[10];
	char *article;
}Sdbtable;


class MysqlDb
{ 
public:
	MysqlDb();
	~MysqlDb();
	bool init(); 
                    
	char* queryArticle();
	char* queryTitle();
private:
	unsigned int key;      
	MYSQL* mysql;
	MYSQL_ROW row;
	MYSQL_RES* result;                                           
};
  
#endif                                                           
 