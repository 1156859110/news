#include "common.h"
#include "cache.h"
int Lru::cachesize = 30000000;//30M
int Lru::cursize = 0;
ListNode * Lru::head = NULL;
ListNode * Lru::tail = NULL;
std::unordered_map<int, ListNode*>Lru::lrumap;
std::mutex Lru::lrumtx;
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
	if((fp = fopen("calendar", "r")) == 0)
	{
		std::cout<<"open failed!"<<std::endl;
		exit(1);
	}
	fseek(fp, 0L, SEEK_END);
	flen = ftell(fp);
	rewind(fp);
	
	std::string reply;
	reply += "HTTP/1.1 200 OK\r\n";    
	reply += "Server: husk's news server\r\n";
	reply += "Content-length:   "+ std::to_string(flen)  + " \r\n";
	reply += "Content-Type: text/html\r\n\r\n";
	reply += "Connection: Keep-Alive\r\n";
	strcpy(data,reply.c_str());

	fread(data + reply.size(), 1, flen , fp);
	fclose(fp);
	return;                 
}
void Lru::getData(int key,char **ppdata,int *psize){
	if (lrumap.find(key) != lrumap.end()){
		ListNode *node = lrumap[key];
		listRemove(node);
		pushFront(node);
		*ppdata  = node->data;
		*psize = 4094;
	}
	else{
		//查询数据库
		char *data = new char[4024];
		ListNode *newNode = new ListNode(key, data);
		//while;
		if (cursize >= cachesize){
			ListNode *node = tail;
			listRemove(node);
			lrumap.erase(node->key);
			delete node->data;
			delete node;
		}
		getCalendar(data) ;
		pushFront(newNode);
		lrumap[key] = newNode;
		*ppdata = data;
		*psize = 4094;
	}
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
void notFind(char *data) 
{
	std::string reply;   
	std::string pages;  
    reply += "HTTP/1.1 404 not find\r\n";    
    reply += "Server: husk's news server\r\n";
    reply += "Content-length:   "+ std::to_string(pages.size())  + " \r\n";
    reply += "Content-Type: text/html\r\n\r\n";
	reply += pages;
    //write(fd, reply.c_str(), strlen(reply.c_str()));                           
   return;                 
}



