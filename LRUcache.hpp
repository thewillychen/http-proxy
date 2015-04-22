#ifndef LRUCACHE_HPP
#define LRUCACHE_HPP
#include <map>
#include <list>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
using namespace std;

class Node{
public:
	char * response; //data = response - http header
	string url; //key = Get including url
	int length;
	Node(char * newResponse, string newUrl){
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

public:
	LRUcache(int size);
	LRUcache();
	char * get(string url); //Returns data or NULL if not found
	int getLength(string url);
	int set(string url, char * response);
	int setLength(string url, int len);

private:
	void moveToFirst(Node node);
};
#endif /* LRUCACHE_HPP */