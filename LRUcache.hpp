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
	string response; //data = response - http header
	string url; //key = url
	Node(string newResponse, string newUrl){
		response = newResponse;
		url = newUrl;
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
	string get(string url); //Returns data or NULL if not found
	int set(string url, string reponse);

private:
	void moveToFirst(Node node);
};
#endif /* LRUCACHE_HPP */