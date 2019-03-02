#include "common.h"
#include "mysqlDb.h"
 
const char HOST[] = "65.49.226.144" ;
const char USER[] = "root" ;
const char PWD[] = "news/123" ;
const char DBNAME[] = "news" ;
const int PORT = 3306;


MysqlDb::MysqlDb():mysql(NULL),row(NULL),result(NULL),skey("0"){
  mysql = mysql_init(NULL);
  if(mysql == NULL) std::cout << "Error: " << mysql_error(mysql);   
  
}
MysqlDb::MysqlDb(std::string sid):mysql(NULL),row(NULL),result(NULL),skey(sid){
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
    if (mysql_set_character_set(mysql, "utf8" ) ) { 
    	std::cout<< "set utf8 error "<<mysql_error(mysql); 
    } 
	return true;
}
 
std::string MysqlDb::queryArticle(){
	std::string sql("SELECT article FROM newstable WHERE id = ");
	sql += skey;
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
		//std::cout << row[0]<<" "<<std::endl;
		mysql_free_result(result);
		return str;
	}
}
std::unordered_map<std::string, OrmTable> MysqlDb::queryTitle()
{
	std::string sql("SELECT id,title,bimg,pubdate FROM newstable WHERE id > ");
	sql += skey;
	//sql += " order by id desc ";
	std::unordered_map<std::string, OrmTable> dbmap;
	if(mysql_query(mysql,sql.c_str())){
		std::cout << "query fail: " << mysql_error(mysql);                                          
	}
	else{
		bool bid = true;
		std::string cursid;
		result = mysql_store_result(mysql);
		//int colnum = mysql_num_fields(result);
		while ((row = mysql_fetch_row (result))){
			//unsigned long *rowlen = mysql_fetch_lengths(result);
			if(bid){
				cursid += row[0];
				bid = false;
			}
			OrmTable otb;
			//std::cout<<row[0]<<" "<<row[1]<<" "<<row[2]<<" "<<row[3]<<" "<<std::endl;
			otb.sid += row[0];
			otb.stitle += row[1];
			otb.bimg = atoi(row[2]);
			otb.spubdate += row[3];
			otb.sarticle = "";
			otb.simg = "";
			dbmap[otb.sid] = otb;
		}
		mysql_free_result(result);
		//return bid?0:atoi(cursid.c_str());
	}
	return dbmap; 
}

