
struct ListNode {
  int key;     
  char *data;   
  ListNode *pre, *next; 
}; 
map<int, ListNode*>hashmap;
ListNode(int k, int v) : key(k), value(v), pre(NULL), next(NULL),head(NULL),tail(NULL),cachesize(30000000) {}
 
//mutuex;
 void listRemove(ListNode *node){
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
 void pushFront(ListNode *node){
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
 char *getData(int key){
  if (hashmap.find(key) != hashmap.end()){
    ListNode *node = hashmap[key];
    listRemove(node);
    pushFront(node);
    return node->data;
  }
  else{
	//查询数据库
	char *data = new char[1024];
	ListNode *newNode = new ListNode(key, data);
    if (toatalsize >= size){
		ListNode *node = tail;
      listRemove(node);
      mp.erase(node->key);
	  delete node->data;
	  delete node;
    }
    pushFront(newNode);
    hashmap[key] = newNode;
  }
} 



string  pages;
void HomePage(int fd) 
{
	string reply;     
    reply += "HTTP/1.1 200 OK\r\n";    
    reply += "Server: husk's news Server\r\n";
    reply += "Content-length:   "+ to_string(pages.size())  + " \r\n";
    reply += "Content-Type: text/html\r\n\r\n";
	reply += "Connection: Keep-Alive\r\n";
	reply += pages;
	
    write(fd, reply.c_str(), strlen(reply.c_str()));                      
 
   return;                 
}
void notFind(int fd) 
{
	string reply;     
    reply += "HTTP/1.1 404 not find\r\n";    
    reply += "Server: husk's news server\r\n";
    reply += "Content-length:   "+ to_string(pages.size())  + " \r\n";
    reply += "Content-Type: text/html\r\n\r\n";
	reply += pages;
    write(fd, reply.c_str(), strlen(reply.c_str()));                           
   return;                 
}



