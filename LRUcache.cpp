#include "LRUcache.hpp"
#include <map>
#include <list>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
using namespace std;

LRUcache::LRUcache(){
}

LRUcache::LRUcache(int size){
	capacity = size;
	usedMemory = 0;
}

char * LRUcache::get(string url){
	if(data.count(url)){
		Node node = data.at(url);
		moveToFirst(node);
		return node.response;
	}
	return NULL;
}

int LRUcache::getLength(string url){
	if(data.count(url)){
		Node node = data.at(url);
		//moveToFirst(node);
		return node.length;
	}
	return NULL;	
}
int LRUcache::set(string url, char * response){
	if(data.count(url)){ //existing element
		Node node = data.at(url);
		moveToFirst(node);
		node.response = response;
		return 1;
	}

	if(strlen(response)< capacity){ //Out of capacity, clear oldest spots until enough space
		while(usedMemory + strlen(response)>capacity){
			string oldKey = queue.back().url;
			queue.pop_back();
			data.erase(oldKey);
		}
	}

	//New entry
	Node node = Node(response,url);
	queue.push_front(node);
	data.insert(std::pair<string, Node>(url, node));
	return 1;
}

int LRUcache::setLength(string url, int len){
	if(data.count(url)){ //existing element
		Node node = data.at(url);
		//moveToFirst(node);
		node.length = len;
		return 1;
	}
	return 0;	
}


void LRUcache::moveToFirst(Node node){
	queue.remove(node);
	queue.push_front(node);
}