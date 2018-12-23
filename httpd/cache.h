#ifndef _CACHE_H_
#define _CACHE_H_

class ListNode {
public:
  int key;     
  char *data;   
  ListNode *pre, *next; 
  ListNode(int k,char *d):key(k),data(d){};
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
public:
   static void listRemove(ListNode *node);
   static void pushFront(ListNode *node);
   static void getCalendar(char *data) ;
   static char *getData(int key);
 
};
#endif




