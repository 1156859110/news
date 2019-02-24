#include "common.h"

#include "cache.h"

int Cache::cachenum = 100;//存放最近100条数据
int Cache::curid = 0;
int visitors = 0;
std::unordered_map<std::string, OrmTable> Cache::newsmap;
std::mutex Cache::Cachemtx;
//c++11不支持读写锁
//std::shared_mutex rwlock;
const int PAGESIZE = 20;
const int TOPN = 5;

//std::lock_guard<std::mutex> locker(Cachemtx);
//mapA.insert(mapB.begin(), mapB.end()) b里面的元素不会更新a
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
//todo 后续改为指针
std::string Cache::getImages(std::string &skey){
	//std::shared_lock<std::shared_mutex> lock(rwlock);
	OrmTable otb = newsmap[skey];
	if(newsmap[skey].pimg == NULL){
		std::string filename = "/root/news/image/"+skey;
		std::ifstream sfp(filename);
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
		return header;
	}else{
		return " ";
	}
} 
std::vector<std::string> Cache::getHtml(std::string &skey){
	
	EKEYTYPE etype = EDEFAULT;
	std::string s2;
	std::vector<std::string>v;
	if(skey == "") return v;
	int key = decodeSkey(skey,etype);
	
	if(etype == EPAGE){
		s2 = getSection2Page(key);
	}
	else if(etype == EARTICLE){
		s2 = getSection2Article(key);
	}
	else{
		//image/css等类型
		v.push_back(getImages(skey));
		return v;
	}
	std::string s1 = getSection1();
	std::string s3 = getSection3();
	std::string s4 = getSection4();
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
	err += "HTTP/1.1 404 OK\r\n";    
	err += "Server: husk's news server\r\n";
	err += "Content-length: 0 \r\n";
	err += "Content-Type: text/html\r\n\r\n";
	return err;                           
}
std::string Cache::getSection1() {
	 std::string s1;
	 s1 +="  <!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\"> ";									
	 s1 +=" <html xmlns=\"http://www.w3.org/1999/xhtml\"><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />";
	 s1 +=" <title>看客</title><meta name=\"keywords\" content=\"看客新闻\" /><meta name=\"description\" content=\"看客新闻\" />";
	 s1 +=" <link href=\"index.css\" rel=\"stylesheet\" type=\"text/css\" /></head><body><div id=\"header\"><div id=\"title\">看客</div>";
	 s1 +=" </div><div id=\"menu\"><a href=\"http://www.kanketech.site\" target=\"_parent\">主页</a></div><div id=\"content_bg\"><div id=\"content\"><div class=\"section_w620 fl\">";
 	return s1;                 
}
std::string Cache::getSection2Page(int key){
	std::string s2;
	s2 += " <div class=\"title_header\">最新动态</div><div class=\"seperate\"></div>";
	if(curid == 0){
		//todo
		MysqlDb *pdb = new MysqlDb("5000");
		if(!pdb->init()){
			//send busy;
		}
		//std::unique_lock<std::shared_mutex> lock(rwlock);
		int id = pdb->queryTitle(newsmap);
		if(id <= 0){
			std::cout<<"no more news"<<'\n';
		}else{
			curid = id;
		}
		std::cout<<" cur id is"<< curid<< '\n';
		delete pdb;
	}
	int start = curid - PAGESIZE *(key -1);
	if(start < 0) start = 0;
	int i = 0;
	//std::shared_lock<std::shared_mutex> lock(rwlock);
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
	s2+=" <div class=\"cleaner\"></div></ul><div class=\"seperate\"></div><div class= \"fr\" > <p>";
	if(key > 1){
		s2+= "<a href=\"page"+std::to_string(key-1)+ "\">上一页</a>";
	}
	s2+= " 第"+std::to_string(key)+"页 ";
	if((key+1)*PAGESIZE > curid)
		s2+= "<a href=\"page1\">下一页</a></p></div>";
	else
		s2+= "<a href=\"page" + std::to_string(key+1)+ "\">下一页</a></p></div>";
	return s2;
} 
std::string Cache::getSection2Article(int key){
	//std::shared_lock<std::shared_mutex> lock(rwlock);
	OrmTable otb = newsmap[std::to_string(key)];
	std::string s2,stemp;
	s2 += " <div class=\"title_header\">";
	s2 +=  otb.stitle;
	s2 +=  "</div>";
	if(otb.bimg){
		s2 += "<div class=\"article_image fr\"> <img src=\"";
		s2 += otb.sid;
		s2 += ".jpg\" alt=\"image\" width=\"300\" height=\"200\" /></div>";
	}
	s2 += "<p>";
	++newsmap[std::to_string(key)].cnt;
	if(otb.particle == NULL){
		MysqlDb *pdb = new MysqlDb(otb.sid);
		if(!pdb->init()){
			//send busy;
		}
		stemp = pdb->queryArticle();
		delete pdb;
	}else{
		stemp = otb.particle;
	}
	s2 += stemp;
	s2 += "</p>";
	return s2;
} 
//这个地方后续做成缓冲比较快。
std::string Cache::getSection3() {
	 std::string s3;
	 
	 s3 += "<div class=\"margin_bottom_40\"></div> <div class=\"section_320 fl margin_right_40\">";
	 s3 += " </div><div class=\"cleaner\"></div></div><div class=\"section_w250 fr\"> ";
	 s3 += "  <div class=\"section_w250_title hot_news_title\">热度排行</div>";
	 s3 += "<div class=\"w250_content\"><div class=\"latest_news\"><ul class=\"list_url\">";
	 
	 struct cmp{
	     bool operator() (const OrmTable a, const OrmTable b ){
	 	    return a.cnt > b.cnt; 
	 	}
	 };
	 std::priority_queue<OrmTable,std::vector<OrmTable>, cmp>hotnews;
	// std::shared_lock<std::shared_mutex> lock(rwlock);
	 for(int i = 0; i< cachenum ; ++i){
		 if(curid - i <= 0) break;
		 OrmTable otb = newsmap[std::to_string(curid - i)];
		 if(i < TOPN){
			 hotnews.push(otb);
			 continue;
		 }
		 if(hotnews.top().cnt < otb.cnt){
			 hotnews.pop();
			 hotnews.push(otb);
		 }
	 }
	 std::vector<std::pair<std::string,std::string>>vss(TOPN);
	 for(int i = 0 ; i < TOPN; ++i){
		 if(!hotnews.empty()){
			 vss[i] = {hotnews.top().sid,hotnews.top().stitle};
			 hotnews.pop();
		 }
	 }
	 for(int i = TOPN - 1; i >=0 ; --i){
		 s3 += "<li><a href=\" article";
		 s3 += vss[i].first;
		 s3 += "\">" ;
		s3 += vss[i].second;
		s3 +=  "</a></li>";
	 }
	 s3 += " </ul></div></div><div class=\"margin_bottom_20\"></div>";
	 s3 += " <div class=\"section_w250_title visitor_title\">访问统计</div> <div class=\"w250_content\">";
	 return s3;
}
 
std::string Cache::getSection4() {
	std::string s4;
	s4 += " <div class=\"title_header\" align = center>";
	s4 += std::to_string(visitors);
	s4 += "</div></div><div class=\"margin_bottom_20\"></div> ";
	s4 += "</div><div class=\"margin_bottom_20\"></div></div></div><div id=\"footer_bg\">";
	s4 += "  <div id=\"footer\"> <a href=\"http://www.miitbeian.gov.cn\">浙ICP备19004077</a> |  Designed by wl";
	s4 += "</div></body></html>";
	return s4;                 
}

void Cache::updateCache(){
	MysqlDb *pdb = new MysqlDb(std::to_string(curid));
	if(!pdb->init()){
		//send busy;
		std::cout<<" access db error\n";
	}
	//需要枷锁读写锁,可以用lock guard
	//std::unique_lock<std::shared_mutex> lock(rwlock);
	int id = pdb->queryTitle(newsmap);
	if(id <= curid){
		std::cout<<"no more news"<<'\n';
	}else{
		curid = id;
	}
	std::cout<<" cur id is"<< curid<< '\n';
	delete pdb;
}		   
void Cache::callback(){
	updateCache();
}		   				   
    



















