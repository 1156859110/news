#include "common.h"
#include "cache.h"
int Lru::cachenum = 100;//存放最近100条数据
int Lru::cursize = 0;
ListNode * Lru::head = NULL;
ListNode * Lru::tail = NULL;
std::unordered_map<int, ListNode*>Lru::lrumap;
std::mutex Lru::lrumtx;

std::vector<pair<char *,int>>Lru::vtitle(40000);
const int PAGESIZE = 10;
enum EKEYTYPE{
	EIMG = 0;
	ETITLE;
	EARTICLE;
	EPAGE;
}
//mapA.insert(mapB.begin(), mapB.end())
void Lru::listRemove(ListNode *node){
	std::lock_guard<std::mutex> locker(lrumtx);
	if (node -> pre != NULL){
		node -> pre -> next = node -> next;
	}
	else{
		head = node -> next;
	}
	if (node -> next != NULL){
		node -> next -> pre = node -> pre;
	}
	else{
		tail = node -> pre;
	}
} 
void Lru::pushFront(ListNode *node){
	std::lock_guard<std::mutex> locker(lrumtx);
	node -> next = head;
	node -> pre = NULL;
	if (head != NULL){
		head -> pre = node;
	}
	head = node;
	if (tail == NULL){
		tail = head;
	}
} 
void Lru::getCalendar(char *data) 
{
	FILE *fp;  
	int flen;
	if((fp = fopen("calendar.html", "r")) == 0)
	{
		std::cout<<"open failed!"<<std::endl;
	}
	fseek(fp, 0L, SEEK_END);
	flen = ftell(fp);
	rewind(fp);
	
	std::string reply;
	reply += "HTTP/1.1 200 OK\r\n";    
	reply += "Server: husk's news server\r\n";
	reply += "Content-length:   "+ std::to_string(flen)  + " \r\n";
	reply += "Content-Type: text/html\r\n";
	reply += "Connection: Keep-Alive\r\n\r\n";
	strncpy(data,reply.c_str(),reply.size());
	std::cout<<flen<<" flen!"<<std::endl;
	fread(data + reply.size(), 1, flen , fp);
	fclose(fp);
	return;                 
}
void Lru::decodeSkey(std::string skey,unsigned int &ukey,EKEYTYPE &etype){
	string ktemp = skey.substr(2,skey.size()-3);
	
	if(memcmp(skey.ctr(),img,3) == 0){
		etype = EIMG;
	}else if(memcmp(skey.ctr(),pag,3) == 0){
		etype = EPAGE;
	}else if(memcmp(skey.ctr(),tit,3) == 0){
		etype = ETITLE;
	}else if(memcmp(skey.ctr(),art,3) == 0){
		etype = EARTICLE;
	}else{
		std::cout<<"error type"<<std::endl;
	}
	  ukey = std::stoul(ktemp,nullptr,0);
	  std::cout << ukey <<" keys: type"<< etype<< '\n';
}
std::vector<std::pair<char *,int size>> Lru::getPage(unsigned int key){
	
	std::vector<std::pair<char *,int size>>v;
	int size = vtitle.size();
	if(size == 0){
		//查数据库
	}
	int start = size - PAGESIZE * key -1;
	if(start < 0) start = 0;
	int i = 0;
	while(start < size && i < PAGESIZE){
		v.push_back([vtitle[start]);
		++start;
		++i;
	}
	return v;
} 
std::vector<std::pair<char *,int size>> Lru::getArticle(unsigned int key){
	
	std::vector<std::pair<char *,int size>>v;
	if (lrumap.find(key) != lrumap.end()){
		ListNode *node = lrumap[ukey];
		listRemove(node);
		pushFront(node);
		v.push_back({node->pdata,node->psize});
	}
	else{
		//查询数据库
		char *pdata;
		ListNode *newNode = new ListNode(ukey, data);
		MysqlDb db(ukey);
		if(!db.init()){
			//send busy;
		}
		pdata = db.queryArticle();
		
		if (cursize >= cachesize){
			ListNode *node = tail;
			listRemove(node);
			lrumap.erase(node->key);
			delete node->data;
			delete node;
		}
		pushFront(newNode);
		lrumap[key] = newNode;
	}
	return v;
} 
std::vector<std::pair<char *,int size>> Lru::getCache(std::string skey){
	
	unsigned int ukey = 0;
	EKEYTYPE etype = 0;
	decodeSkey(skey,ukey,etype);
	std::vector<std::pair<char *,int size>>v;
	
	if(etype == EPAGE){
		
	}
	else if(){
		
	}
	else if(){
		
	}
	else{
		
	}
	
	return v;
} 

void getHomePage(char *data,char *src,int len) 
{
	std::string reply;
	reply += "HTTP/1.1 200 OK\r\n";    
	reply += "Server: husk's news server\r\n";
	reply += "Content-length:   "+ std::to_string(len)  + " \r\n";
	reply += "Content-Type: text/html\r\n\r\n";
	reply += "Connection: Keep-Alive\r\n";
	strcpy(data,reply.c_str());
	strncpy(data+reply.size(),src,len) ;   
	return;                 
}
void notFind(char **p,int *plen) 
{
	char data[] = "HTTP/1.1 404 not find\r\n
		Server: husk's news server\r\n
		Content-length: 0 \r\n
		Content-Type: text/html\r\n\r\n";
	*plen = sizeof(data)+1;
    **pp = new char[*plen]();  
	memcpy(*pp,data,*plen);  
	*pp[*plen]  = '\0';           
   return;                 
}



