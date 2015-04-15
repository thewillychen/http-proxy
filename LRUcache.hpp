#include <map>
#include <list>
#include <string>
using namespace std;

class Node{
public:
	string data; //data = response - http header
	string key; //key = url
	//Node previous;
	//Node next;
};

class LRUcache{
	int capacity;
	map<int, Node> data;
	list<Node> queue;
	//Node head;
	//Node tail;

public:
	LRUcache(int size);
	string get(string key);
	int set(string key, string data);

private:
	void add(Node node); //Append node as head
	void remove(Node node);
	void moveToFirst(Node node);
	void removeLast();
};