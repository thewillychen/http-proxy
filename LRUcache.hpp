#include <map>
#include <list>
#include <string>
using namespace std;

class Node{
public:
	string data; //data = response - http header
	string key; //key = url
};

class LRUcache{
	int capacity;
	int usedMemory;
	map<int, Node> data;
	list<Node> queue;

public:
	LRUcache(int size);
	string get(string key); //Returns data or NULL if not found
	int set(string key, string data);

private:
	void moveToFirst(Node node);
};