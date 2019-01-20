#ifndef _CACHE_H_
#define _CACHE_H_

class ListNode {
public:
  int key;     
  char *pdata; 
  int psize;  
  ListNode *pre, *next; 
  ListNode(int k,char *d,int size):key(k),data(d),psize(size){};
  //~ListNode();
}; 

class Lru{
private:
	static std::unordered_map<int, ListNode*>lrumap;
	static ListNode *head;
	static ListNode *tail; 
	static int cachesize;
	static std::mutex lrumtx;
	static int cursize;
	static std::vector<std::pair<char*,int >>vtitle;
public:
	
   static void listRemove(ListNode *node);
   static void pushFront(ListNode *node);
   static void getCalendar(char *data) ;
   static void getData(int key,char **ppdata,int *psize);
};
#endif




