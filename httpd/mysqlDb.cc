#include "common.h"
#include "mysqlDb.h"
 
const char HOST[] = "65.49.226.144" ;
const char USER[] = "root" ;
const char PWD[] = "news/123" ;
const char DBNAME[] = "news" ;
const int PORT = 3306;
MysqlDb::MysqlDb():mysql(NULL),row(NULL),result(NULL),key(0){
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
 
char* MysqlDb::queryArticle(){
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
		char *particle = new char[row[0].size()];
		strcpy(particle,row[0]);
		std::cout << row[0]<< row[0].size()<<std::endl;
		mysql_free_result(result);
		return particle;
	}
}
std::unordered_map<int, Sdbtable> MysqlDb::queryTitle()
{
	std::string sql("SELECT id,title,bimg,pubdate FROM newstable WHERE id > ");
	sql += std::to_string (key);
	
	if(mysql_query(mysql,sql.c_str())){
		std::cout << "query fail: " << mysql_error(mysql);   
		return NULL;                                          
	}
	else{
		Sdbtable sdb;
		std::unordered_map<int, Sdbtable>dbmap;
		result = mysql_store_result(mysql);
		//int colnum = mysql_num_fields(result);
		while ((row = mysql_fetch_row (result))){
			std::cout<<row[0]<<" "<<row[1]<<" "<<row[2]<<" "<<row[3]<<" "<<std::endl;
			memset(&sdb,0,sizeof(sdb));
			sdb.id = row[0] ;
			strncpy(sdb.title,row[1],49);
			sdb.title[49] = '\0';
			sdb.bimg = row[2];
			strncpy(sdb.pubdate,row[3],9);
			sdb.pubdate[9] = '\0';
			sdb.article = NULL;
			dbmap[sdb.id] = sdb;
		}
		mysql_free_result(result);
		return dbmap;
	}
}
