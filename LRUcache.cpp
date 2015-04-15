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

string LRUcache::get(string key){
	if(data.count(key)){
		Node node = data.at(key);
		moveToFirst(node);
		return node.data;
	}
	return NULL;
}

int LRUcache::set(string key, string data){
	if(data.count(key)){ //existing element
		Node node = data.at(key);
		moveToFirst(node);
		node.data = data;
		return 1;
	}

	if(data.length()< capacity){ //Out of capacity, clear oldest spots until enough space
		while(usedMemory + data.length()>capacity){
			string oldKey = queue.back().key;
			queue.pop_back();
			data.erase(oldKey);
		}
	}

	//New entry
	Node node = Node(data,key);
	queue.push_front(node);
	data.insert(std::pair<string, string>(key, node));
	return 1;
}


void LRUcache::moveToFirst(Node node){
	queue.remove(node);
	queue.push_front(node);
}