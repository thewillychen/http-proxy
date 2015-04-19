#include "LRUcache.hpp"
#include <map>
#include <list>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <mutex>
using namespace std;

LRUcache::LRUcache(){
}

LRUcache::LRUcache(int size){
	capacity = size;
	usedMemory = 0;
}

string LRUcache::get(string url){
	mtx.lock();
	if(data.count(url)){
		Node node = data.at(url);
		mtx.unlock();
		moveToFirst(node);
		return node.response;
	}
	mtx.unlock();
	return NULL;
}

int LRUcache::set(string url, string response){
	mtx.lock();
	if(data.count(url)){ //existing element
		Node node = data.at(url);
		mtx.unlock();
		moveToFirst(node);
		mtx.lock();
		node.response = response;
		mtx.unlock();
		return 1;
	}

	if(response.length()< capacity){ //Out of capacity, clear oldest spots until enough space
		while(usedMemory + response.length()>capacity){
			string oldKey = queue.back().url;
			queue.pop_back();
			data.erase(oldKey);
		}
	}

	//New entry
	Node node = Node(response,url);
	queue.push_front(node);
	data.insert(std::pair<string, Node>(url, node));
	mtx.unlock();
	return 1;
}


void LRUcache::moveToFirst(Node node){
	mtx.lock();
	queue.remove(node);
	queue.push_front(node);
	mtx.unlock();
}