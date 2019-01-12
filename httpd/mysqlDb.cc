#include "common.h"
#include "mysqlDb.h"
 
const char HOST[] = "65.49.226.144" ;
const char USER[] = "root" ;
const char PWD[] = "news/123" ;
const char DBNAME[] = "news" ;
const int PORT = 3306;
MysqlDb::MysqlDb():mysql(NULL),row(NULL),result(NULL){
  mysql = mysql_init(NULL);
  if(mysql == NULL) std::cout << "Error: " << mysql_error(mysql);   
}
MysqlDb::~MysqlDb(){                                                           
  if(mysql) mysql_close(mysql);
}
 
bool MysqlDb::init(){
	if(mysql == NULL) {
		std::cout << "construct error: "<<std::endl;
		return false;
	}
	if(!mysql_real_connect(mysql,HOST,USER,PWD,DBNAME,PORT,NULL,0)){
		std::cout<< "connect error " << mysql_error(mysql);
		return false;
	}
	return true;
}
 
char* MysqlDb::queryArticle(unsigned int key){
	std::string sql("SELECT article FROM newstable WHERE id = ");
	sql += std::to_string (key);
	//返回0表示成功。
	if(mysql_query(mysql,sql.c_str())){
		std::cout << "query fail: " << mysql_error(mysql)<<std::endl;  
		return NULL;                                     
	}
	else{
		result = mysql_store_result(mysql);
		row = mysql_fetch_row(result);
		std::cout << row[0] << "\t\t"<<std::endl;
		mysql_free_result(result);
	}
	return NULL;
}
char* MysqlDb::queryTitle(unsigned int key)
{
	std::string sql("SELECT title,date FROM newstable WHERE id > ");
	sql += std::to_string (key);
	
	if(mysql_query(mysql,sql.c_str())){
		std::cout << "query fail: " << mysql_error(mysql);   
		return NULL;                                          
	}
	else{
		result = mysql_store_result(mysql);
		int rownum = mysql_num_rows(result);
		int colnum = mysql_num_fields(result);
		for(int i = 0; i < rownum; ++i){
			row = mysql_fetch_row(result);
			if(row <= 0) continue;
			for(int j=0; j<colnum; j++){
				std::cout << row[j] << "\t\t";
			}
			std::cout << std::endl;
		}
		mysql_free_result(result);
	}
	return NULL;
}
