#include "LRUcache.hpp"
#include <map>
#include <list>
#include <string>
using namespace std;

LRUcache::LRUcache(int size){
	capacity = size;
}

string LRUcache::get(string key){
	if(data.count(key)){

	}
}

int set(string key, string data){
	if(data.count(key)){

	}
}

void LRUcache::add(Node node){
	node->next = NULL;
	node->previous = NULL;

	if(head == NULL){
		head = node;
		end = node;
		return;
	}

	head->previous = node;
	node->next = head;
	head = node;
}

// void LRUcache::remove(Node node){
// 	if(node == NULL || head == NULL)
// 		return;
// 	if(head == end && head == node){ //only node
// 		head = NULL;
// 		end = NULL;
// 		return;
// 	}
// 	if(node == head){ //remove from head
// 		head->next->previous = NULL;
// 		head = head->next;
// 		return;
// 	}
// 	if(node == end){//remove from end
// 		end->previous->next = NULL;
// 		end = end->previous;
// 		return;
// 	}

// 	node->previous->next = node->next;
// 	node->next->previous = node->previous;
// }

void LRUcache::moveToFirst(Node node){
	queue.remove(node);
	queue.push_front(node);
	//remove(node);
	//add(node);
}

// void LRUcache::removeLast(){
// 	remove(end);
// }