#include "common.h"

#include "cache.h"

int Lru::cachenum = 100;//存放最近100条数据
int Lru::curid = 0;
int visitors = 12323;
std::unordered_map<std::string, Sdbtable> Lru::newsmap;
std::mutex Lru::lrumtx;

const int PAGESIZE = 10;

//std::lock_guard<std::mutex> locker(lrumtx);
//mapA.insert(mapB.begin(), mapB.end()) b里面的元素不会更新a
int Lru::decodeSkey(std::string &skey,EKEYTYPE &etype){
	int size = skey.size();
	if(size ==0) return 0;
	if(memcmp(skey.c_str(),"img",3) == 0){
		etype = EIMG;
	}else if(memcmp(skey.c_str(),"pag",3) == 0){
		etype = EPAGE;
	}else if(memcmp(skey.c_str(),"art",3) == 0){
		etype = EARTICLE;
	}else{
		etype =  EDEFAULT;
		std::cout<<"no type"<<std::endl;
	}
	std::cout <<" keys: type"<< etype<< '\n';
	return 0;
}



std::string Lru::getImg(std::string &skey){
	Sdbtable stab = newsmap[skey];
	if(!stab.bimg) return " ";
	if(newsmap[skey].pimg == NULL){
#if 0 
		FILE *fp;  
		int flen;
		if((fp = fopen(skey, "r")) == 0){
		std::cout<<"open failed!"<<std::endl;
		}
		fseek(fp, 0L, SEEK_END);
		flen = ftell(fp);
		rewind(fp);
		std::string header;
		header += "HTTP/1.1 200 OK\r\nServer: husk's news server\r\n";
		header += "Content-length:   "+ std::to_string(flen)  + " \r\n";
		header += "Content-Type: image/jpg\r\nConnection: Keep-Alive\r\n\r\n";
		int size = header.size();
		char *pimg = new char[flen + size]();
		strncpy(pimg,header.c_str(),size);
		std::cout<<flen<<" flen!"<<std::endl;
		fread(pimg + size, 1, flen, fp);
		fclose(fp);  
#endif
		std::ifstream sfp(skey);
		std::stringstream temp;
		temp << sfp.rdbuf();
		return temp.str();
	}else{
		return " ";
	}
} 

std::vector<std::string> Lru::getHtml(std::string &skey){
	
	EKEYTYPE etype = EDEFAULT;
	decodeSkey(skey,etype);
	std::vector<std::string>v;
#if 0
	if(etype == EPAGE){
		
	}
	else if(etype == EIMG){
		 Lru::getImg(skey);
	}
	else if(etype == EARTICLE){
		Lru::getArticle(skey,key);
	}
	else{
		// Lru::getImg(skey);
	}
#endif
	
	std::string s1 = getSection1();
	std::string s2 = getSection2Page(1);
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

std::string Lru::getHeader(int len) 
{
	std::string header;
	header += "HTTP/1.1 200 OK\r\n";    
	header += "Server: husk's news server\r\n";
	header += "Content-length:   "+ std::to_string(len)  + " \r\n";
	header += "Content-Type: text/html\r\n\r\n";
	header += "Connection: Keep-Alive\r\n";
	return header;                 
}
std::string Lru::getErr() 
{
	std::string err;
	err += "HTTP/1.1 404 OK\r\n";    
	err += "Server: husk's news server\r\n";
	err += "Content-length: 0 \r\n";
	err += "Content-Type: text/html\r\n\r\n";
	return err;                           
}
std::string Lru::getSection1() {
	 std::string s1;
	 s1 +="  <!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\"> ";									
	 s1 +=" <html xmlns=\"http://www.w3.org/1999/xhtml\"><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />";
	 s1 +=" <title>看客</title><meta name=\"keywords\" content=\"中港台新闻\" /><meta name=\"description\" content=\"中国，香港，台湾相关的新闻\" />";
	 s1 +=" <link href=\"index.css\" rel=\"stylesheet\" type=\"text/css\" /></head><body><div id=\"header\"><div id=\"title\">看客</div>";
	 s1 +=" </div><div id=\"menu\"><a href=\"http://www.kanketech.site\" target=\"_parent\">主页</a></div><div id=\"content_bg\"><div id=\"content\"><div class=\"section_w620 fl\">";
 	return s1;                 
}
std::string Lru::getSection2Page(int key){
	std::string s2;
	s2 += " <div class=\"title_header\">最新动态</div><div class=\"seperate\"></div><ul class=\"list_url\">";
	if(curid == 0){
		MysqlDb *pdb = new MysqlDb(1200);
		if(!pdb->init()){
			//send busy;
		}
		//需要枷锁,curid,不能用size，后续需要改动
		newsmap = pdb->queryTitle();
		curid = newsmap.size();
		std::cout<<" cur id is"<< curid<< '\n';
		delete pdb;
	}
	int start = curid - PAGESIZE * key -1;
	if(start < 0) start = 0;
	int i = 0;
	while(start < curid && i < PAGESIZE){
		Sdbtable stab = newsmap[std::to_string(start)];
		s2+= "<li><a href=\"art";
		s2+= stab.id;
		s2+= "\">";
		s2+= stab.title;
		s2+= "</a> <span>";
		s2+= stab.pubdate;
		s2+= "</span></li>";
		++start;
		++i;
	}
	s2+=" <div class=\"cleaner\"></div></ul><div class=\"seperate\"></div><div class= \"fr\" > <p>页次：";
	s2+=std::to_string(key)+"/"+std::to_string(curid/10);
	if(key >= 1){
		s2+= "<a href=\" "+std::to_string(key-1)+ "\"><<</a>";
	}
	s2+= std::to_string(key);
	s2+= "<a href=\" " + std::to_string(key+1)+ "\">>></a></p></div>";
	return s2;
} 
std::string Lru::getSection2Article(std::string &skey){
	Sdbtable stab = newsmap[skey];
	std::string s2,stemp;
	s2 += " <div class=\"title_header\">";
	s2 +=  stab.title;
	s2 +=  "</div>";
	if(stab.bimg){
		s2 += "<div class=\"article_image fr\"> <img src=\"images/";
		s2 += std::to_string(stab.id);
		s2 += ".jpg\" alt=\"image\" width=\"300\" height=\"200\" /></div>";
	}
	s2 += "<p>";
	if(newsmap[skey].particle == NULL){
		MysqlDb *pdb = new MysqlDb(stab.id);
		if(!pdb->init()){
			//send busy;
		}
		stemp = pdb->queryArticle();
		delete pdb;
	}else{
		stemp = newsmap[skey].particle;
	}
	s2 += stemp;
	s2 += "</p>";
	return s2;
} 
std::string Lru::getSection3() {
	 std::string s3;
	 
	 s3 += "<div class=\"margin_bottom_40\"></div> <div class=\"section_320 fl margin_right_40\">";
	 s3 += " </div><div class=\"cleaner\"></div></div><div class=\"section_w250 fr\"> ";
	 s3 += "  <div class=\"section_w250_title hot_news_title\">热度排行</div>";
	 s3 += "<div class=\"w250_content\"><div class=\"latest_news\"><ul class=\"list_url\">";
	 s3 += "<li><a href=\" ";
	 int title = 10;
	 //todo待计算排名前5的数据
	 s3 += std::to_string(title);
	 s3 += "\">";
	 s3 += std::to_string(title);
	 s3 += " </a></li></ul></div></div><div class=\"margin_bottom_20\"></div>";
	 s3 += " <div class=\"section_w250_title visitor_title\">访问统计</div> <div class=\"w250_content\">";
	 return s3;
}
 
std::string Lru::getSection4() {
	std::string s4;
	s4 += " <div class=\"title_header\" align = center>";
	s4 += std::to_string(visitors);
	s4 += "</div></div><div class=\"margin_bottom_20\"></div> ";
	s4 += "</div><div class=\"margin_bottom_20\"></div></div></div><div id=\"footer_bg\">";
	s4 += "  <div id=\"footer\"> <a href=\"http://www.miitbeian.gov.cn\">浙ICP备19004077</a> |  Designed by wl";
	s4 += "</div></body></html>";
	return s4;                 
}

		   
				   
    



















