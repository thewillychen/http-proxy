#ifndef LRUCACHE_HPP
#define LRUCACHE_HPP
#include <map>
#include <list>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
using namespace std;

class Node{
public:
	string data; //data = response - http header
	string key; //key = url
	Node(string data, string key){
		data = data;
		key = key;
	}
};

class LRUcache{
	int capacity;
	int usedMemory;
	map<int, Node> data;
	list<Node> queue;

public:
	LRUcache(int size);
	LRUcache();
	string get(string key); //Returns data or NULL if not found
	int set(string key, string data);

private:
	void moveToFirst(Node node);
};
#endif /* LRUCACHE_HPP */