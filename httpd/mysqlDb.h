#ifndef _MYSQLDB_H_
#define _MYSQLDB_H_
 
#include <mysql/mysql.h>

class MysqlDb
{ 
public:
  MysqlDb();
  ~MysqlDb();
  bool init();                          
 char* queryArticle(unsigned int key);
 char* queryTitle(unsigned int key);
private:
  MYSQL* mysql;
  MYSQL_ROW row;
  MYSQL_RES* result;                                           
};
  
#endif                                                           
 