#ifndef LRUCACHE_HPP
#define LRUCACHE_HPP
#include <map>
#include <list>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <mutex> 
using namespace std;

class Node{
public:
	string response; //data = response - http header
	string url; //key = url
	Node(string newResponse, string newUrl){
		response = newResponse;
		url = newUrl;
	}

	bool operator==( const Node& other ) const  {
		return(response==other.response && url==other.url);
	}
};

class LRUcache{
	int capacity;
	int usedMemory;
	map<string, Node> data;
	list<Node> queue;
	mutex mtx;

public:
	LRUcache(int size);
	LRUcache();
	string get(string url); //Returns data or NULL if not found
	int set(string url, string response);

private:
	void moveToFirst(Node node);
};
#endif /* LRUCACHE_HPP */