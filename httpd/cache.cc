#include "common.h"
#include "mysqlDb.h"
#include "cache.h"

int Cache::cachenum = 100;//存放最近100条数据,preid---curid之间的数据
int Cache::curid = 0;
int Cache::preid = 0;
int visitors = 0;
std::unordered_map<std::string, OrmTable> Cache::newsmap;
std::mutex Cache::mtx;
//c++11不支持读写锁，用linux pthread实现
//pthread_rwlock_t rwlock;
//pthread_rwlock_init(&rwlock,NULL);
//pthread_rwlock_rdlock(&rwlock);//获取读取锁
//pthread_rwlock_unlock(&rwlock);   
//pthread_rwlock_wrlock(&rwlock);
//pthread_rwlock_unlock(&rwlock);
//std::shared_mutex rwlock;

const int PAGESIZE = 20;
const int TOPN = 5;


int Cache::decodeSkey(std::string &skey,EKEYTYPE &etype){
	int size = skey.size();
	int key = 0;
	if(size == 0){
		//返回首页
		etype = EPAGE;
		key = 1;
	}else if(memcmp(skey.c_str(),"page",4) == 0){
		etype = EPAGE;
		key = atoi(skey.substr(4).c_str());
	}else if(memcmp(skey.c_str(),"article",7) == 0){
		etype = EARTICLE;
		key = atoi(skey.substr(7).c_str());
	}else{
		etype = EIMG;
	}
	if(etype != EIMG) ++visitors;
	std::cout <<key <<" keys: type "<< etype<< '\n';
	return key;
}
std::string Cache::getImages(std::string &skey){
	OrmTable otb;
	//std::shared_lock<std::shared_mutex> lock(rwlock);
	{
		std::lock_guard<std::mutex> locker(mtx);
		if(newsmap.find(skey) != newsmap.end()){
			otb = newsmap[skey];
		}
	}
	if(otb.simg == ""){
		std::string filename = "/root/news/image/"+skey;
		std::ifstream sfp(filename,std::ifstream::in);
		if(sfp.is_open()){
			std::stringstream temp;
			temp << sfp.rdbuf();
			std::string header;
			header += "HTTP/1.1 200 OK\r\nServer: husk's news server\r\n";
			header += "Cache-control: only-if-cached\r\n";
			header += "Content-length:   "+ std::to_string(temp.str().size())  + " \r\n";
			if(skey.find(".jpg") != std::string::npos){
				header += "Content-Type: image/jpg\r\nConnection: Keep-Alive\r\n\r\n";
			}else if(skey.find(".css") != std::string::npos){
				header += "Content-Type: text/css\r\nConnection: Keep-Alive\r\n\r\n";
			}else{
				header += "Content-Type: text/html\r\nConnection: Keep-Alive\r\n\r\n";
			}
			header += temp.str();
			//atoi直接返回已经读取转换后多整数。如123.jpg == 123
			int ikey = atoi(skey.c_str());
			if(ikey > preid || ikey == 0){
				std::lock_guard<std::mutex> locker(mtx);
				newsmap[skey].simg = header;
				newsmap[skey].bimg = true;
			}
			return header;
		}else{
			return getErr();
		}
	}else{
		return otb.simg;
	}
}
std::vector<std::string> Cache::getHtml(std::string &skey){
	
	EKEYTYPE etype = EDEFAULT;
	std::string s2;
	std::vector<std::string>v;
	if(skey == "") return v;
	int key = decodeSkey(skey,etype);
	
	if(etype == EPAGE){
		if(getSection2Page(key,s2)!=0){
			v.push_back(getErr());
			return v;
		}
	}
	else if(etype == EARTICLE){
		if(getSection2Article(key,s2)!=0){
			v.push_back(getErr());
			return v;
		}
	}
	else{
		//image/css等类型
		v.push_back(getImages(skey));
		return v;
	}
	std::string s1;
	getSection1(s1);
	std::string s3;
	getSection3(s3);
	std::string s4;
	getSection4(s4);
	std::string s0 = getHeader(s1.size()+s2.size()+s3.size()+s4.size());
	v.push_back(s0);
	v.push_back(s1);
	v.push_back(s2);
	v.push_back(s3);
	v.push_back(s4);
	return v;
} 

std::string Cache::getHeader(int len) 
{
	std::string header;
	header += "HTTP/1.1 200 OK\r\n";    
	header += "Server: husk's news server\r\n";
	header += "Content-length:   "+ std::to_string(len)  + " \r\n";
	header += "Content-Type: text/html\r\n";
	header += "Cache-control: no-cache\r\n";
	header += "Connection: Keep-Alive\r\n\r\n";
	return header;                 
}
std::string Cache::getErr() 
{
	std::string err;
	err += "HTTP/1.1 404 not found\r\n";    
	err += "Server: husk's news server\r\n";
	err += "Content-length: 0 \r\n";
	err += "Content-Type: text/html\r\n\r\n";
	return err;                           
}
int Cache::getSection1(std::string &s1) {
	 s1 +="  <!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\"> ";									
	 s1 +=" <html xmlns=\"http://www.w3.org/1999/xhtml\"><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />";
	 s1 +=" <title>看客</title><meta name=\"keywords\" content=\"看客新闻\" /><meta name=\"description\" content=\"看客新闻\" />";
	 s1 +=" <link href=\"index.css\" rel=\"stylesheet\" type=\"text/css\" /></head><body><div id=\"header\"><div id=\"title\">看客</div>";
	 s1 +=" </div><div id=\"menu\"><a href=\"http://www.kanketech.site\" target=\"_parent\">主页</a></div><div id=\"content_bg\"><div id=\"content\"><div class=\"section_w620 fl\">";
 	return 0;                 
}
int Cache::getSection2Page(int key,std::string &s2){
	s2 += " <div class=\"title_header\">最新动态</div><div class=\"seperate\"></div>";
	if(curid == 0){
		updateCache();
	}
	int start = curid - PAGESIZE *(key -1);
	if(key < 1 || start < 0) return -1;
	int i = 0;
	//std::shared_lock<std::shared_mutex> lock(rwlock);
	{
		std::lock_guard<std::mutex> locker(mtx);
		while(start >0 && i < PAGESIZE){
			OrmTable otb = newsmap[std::to_string(start)];
			if(i == 0) s2 += "<ul class=\"list_url\">";
			//std::cout<<" sid start"<<start;
			//std::cout<<otb.sid<<" sid "<<otb.stitle<<" stitle "<<otb.spubdate<<" spubdate"<<std::endl;
			s2+= "<li><a href=\"article";
			s2+= otb.sid;
			s2+= "\">";
			s2+= otb.stitle;
			s2+= "</a> <span>";
			s2+= otb.spubdate;
			s2+= "</span></li>";
			--start;
			++i;
		}
	}
	
	s2+=" <div class=\"cleaner\"></div></ul><div class=\"seperate\"></div><div class= \"fr\" > <p>";
	if(key > 1){
		s2+= "<a href=\"page"+std::to_string(key-1)+ "\">上一页</a>";
	}
	s2+= " 第"+std::to_string(key)+"页 ";
	if((key+1)*PAGESIZE > curid)
		s2+= "<a href=\"page1\">下一页</a></p></div>";
	else
		s2+= "<a href=\"page" + std::to_string(key+1)+ "\">下一页</a></p></div>";
	return 0;
} 
 int Cache::getSection2Article(int key,std::string &s2){
	std::string skey = std::to_string(key);
	std::string stemp;
	OrmTable otb;
	//std::shared_lock<std::shared_mutex> lock(rwlock);
	{
		std::lock_guard<std::mutex> locker(mtx);
		if(newsmap.find(skey) == newsmap.end()){
			return -1;
		}
		otb = newsmap[skey];
		++newsmap[skey].cnt;
	}
	if(otb.sarticle == ""){
		MysqlDb *pdb = new MysqlDb(otb.sid);
		if(!pdb->init()){
			//send busy;
		}
		stemp = pdb->queryArticle();
		delete pdb;
	}else{
		s2 = otb.sarticle;
		return 0;
	}
	s2 += " <div class=\"title_header\">";
	s2 +=  otb.stitle;
	s2 +=  "</div>";
	if(otb.bimg){
		s2 += "<div class=\"article_image fr\"> <img src=\"";
		s2 += otb.sid;
		s2 += ".jpg\" alt=\"image\" width=\"300\" height=\"200\" /></div>";
	}
	s2 += "<p>";
	s2 += stemp;
	s2 += "</p>";
	if(key > preid){
		std::lock_guard<std::mutex> locker(mtx);
		newsmap[std::to_string(key)].sarticle = s2;
	}
	return 0;
} 
int Cache::getSection3(std::string &s3) {
	 
	 s3 += "<div class=\"margin_bottom_40\"></div> <div class=\"section_320 fl margin_right_40\">";
	 s3 += " </div><div class=\"cleaner\"></div></div><div class=\"section_w250 fr\"> ";
	 s3 += "  <div class=\"section_w250_title hot_news_title\">热度排行</div>";
	 s3 += "<div class=\"w250_content\"><div class=\"latest_news\"><ul class=\"list_url\">";
	 
	 struct cmp{
	     bool operator() (const std::pair<std::string,int> a, const std::pair<std::string,int> b ){
	 	    return a.second > b.second; 
	 	}
	 };
	 std::priority_queue<std::pair<std::string,int>,std::vector<std::pair<std::string,int>>, cmp>hotnews;
	 std::pair<std::string,int>tempair;
	// std::shared_lock<std::shared_mutex> lock(rwlock);
	 for(int i = 0; i< cachenum ; ++i){
		 if(curid - i <= 0){
		 	 break;
		 }
		 //
		 {
			 std::string skey = std::to_string(curid - i);
			 std::lock_guard<std::mutex> locker(mtx);
			 if(newsmap.find(skey) == newsmap.end()){
				 continue;
			 }
			 tempair = {skey,newsmap[skey].cnt};
		 }
		 if(i < TOPN){
			 hotnews.push(tempair);
			 continue;
		 }
		 if(hotnews.top().second < tempair.second){
			 hotnews.pop();
			 hotnews.push(tempair);
		 }
	 }
	 std::vector<std::string>vs(TOPN);
	 for(int i = 0 ; i < TOPN; ++i){
		 if(!hotnews.empty()){
			 vs[i] = hotnews.top().first;
			 hotnews.pop();
		 }
	 }
	 {
		 std::lock_guard<std::mutex> locker(mtx);
		 for(int i = TOPN - 1; i >=0 ; --i){
			 s3 += "<li><a href=\" article";
			 s3 += vs[i];
			 s3 += "\">" ;
			 s3 += newsmap[vs[i]].stitle;
			 s3 +=  "</a></li>";
		 }
	 }
	 s3 += " </ul></div></div><div class=\"margin_bottom_20\"></div>";
	 s3 += " <div class=\"section_w250_title visitor_title\">访问统计</div> <div class=\"w250_content\">";
	 return 0;
}
 
int Cache::getSection4(std::string &s4) {
	s4 += " <div class=\"title_header\" align = center>";
	s4 += std::to_string(visitors);
	s4 += "</div></div><div class=\"margin_bottom_20\"></div> ";
	s4 += "</div><div class=\"margin_bottom_20\"></div></div></div><div id=\"footer_bg\">";
	s4 += "  <div id=\"footer\"> <a href=\"http://www.miitbeian.gov.cn\">浙ICP备19004077</a> |  Designed by wl";
	s4 += "</div></body></html>";
	return 0;                 
}

void Cache::updateCache(){
	MysqlDb *pdb = new MysqlDb(std::to_string(curid));
	if(!pdb->init()){
		//send busy;
		std::cout<<" access db error\n";
	}
	//std::unique_lock<std::shared_mutex> lock(rwlock);
	std::unordered_map<std::string,OrmTable> tempmap = pdb->queryTitle();
	delete pdb;
	int id = tempmap.size();
	if(id <= 0){
		std::cout<<"no more news"<<'\n';
	}else{
		std::lock_guard<std::mutex> locker(mtx);
		//tempmap里面的元素不会更新newsmap,如果重复
		newsmap.insert(tempmap.begin(), tempmap.end());
		curid += id;
		int temid = curid - cachenum;
		//删除preid之前的缓存。
		if(temid > preid){
			for(int i = preid; i < temid;++i){
				std::string skey = std::to_string(i);
				if(newsmap.find(skey)!= newsmap.end()){
					newsmap[skey].sarticle = "";
					newsmap[skey].simg = "";
				}
			}
			preid = temid;
		}
	}
	std::cout<<" cur id is"<< curid<<"pre id is "<<preid<<'\n';
}		   
void Cache::callback(){
	updateCache();
}		 


