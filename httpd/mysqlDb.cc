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
MysqlDb::MysqlDb(int curid):mysql(NULL),row(NULL),result(NULL),key(curid){
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
 
std::string MysqlDb::queryArticle(){
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
		std::string str;
		str += row[0];
		//strcpy(particle,row[0]);
		std::cout << row[0]<<" "<<std::endl;
		mysql_free_result(result);
		return str;
	}
}
std::unordered_map<std::string, Sdbtable> MysqlDb::queryTitle()
{
	std::unordered_map<std::string, Sdbtable>dbmap;
	std::string sql("SELECT id,title,bimg,pubdate FROM newstable WHERE id > ");
	sql += std::to_string (key);
	
	if(mysql_query(mysql,sql.c_str())){
		std::cout << "query fail: " << mysql_error(mysql);   
		return dbmap;                                          
	}
	else{
		Sdbtable sdb;
		
		result = mysql_store_result(mysql);
		//int colnum = mysql_num_fields(result);
		while ((row = mysql_fetch_row (result))){
			std::cout<<row[0]<<" "<<row[1]<<" "<<row[2]<<" "<<row[3]<<" "<<std::endl;
			memset(&sdb,0,sizeof(sdb));
			sdb.id = atoi(row[0]);
			strncpy(sdb.title,row[1],49);
			sdb.title[49] = '\0';
			sdb.bimg = row[2];
			strncpy(sdb.pubdate,row[3],9);
			sdb.pubdate[9] = '\0';
			sdb.particle = NULL;
			sdb.pimg = NULL;
			dbmap[std::to_string(sdb.id)] = sdb;
		}
		mysql_free_result(result);
		return dbmap;
	}
}
